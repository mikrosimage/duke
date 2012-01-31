#include "ShadingGraphCodeBuilder.h"

#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>

#include <iostream>
#include <sstream>

#include <tr1/functional_hash.h>

#include <boost/ptr_container/ptr_vector.hpp>

using namespace ::std;
using namespace ::duke::protocol;
using namespace ::google::protobuf;

namespace shader_factory {

static inline string placeHolder(const char placeHolderChar, const int i) {
    ostringstream str;
    str << placeHolderChar << i;
    return str.str();
}

static inline string placeHolder(const char* string, const int i) {
    ostringstream str;
    str << string << i;
    return str.str();
}

static inline string argPlaceHolder(const int i) {
    return placeHolder("_a", i);
}
static inline string varPlaceHolder(const int i) {
    return placeHolder("_var", i);
}

class Function;

struct IFunctionGetter {
    virtual const string& getId(const FunctionInstance& instance) const = 0;
    virtual const string& getReturnType(const FunctionInstance& instance) const = 0;
};

class Function {
    const string m_Id;
    const FunctionInstance &m_Instance;
    const FunctionPrototype *m_pPrototype;
    const FunctionSignature *m_pSignature;

    void prependStrings(ostream &stream) const {
        for (int i = 0; i < m_pSignature->prependdeclaration_size(); ++i)
            stream << m_pSignature->prependdeclaration(i) << endl;
    }

    void prependParameters(ostream &stream) const {
        for (int i = 0; i < m_pSignature->parametertype_size(); ++i)
            stream << m_pSignature->parametertype(i) << ' ' << m_Instance.parametername(i) << ';' << endl;
    }

    void writeSignature(ostream &stream) const {
        stream << m_pSignature->returntype() << ' ' << m_Id << '(';
        for (int i = 0; i < m_pSignature->operandtype_size(); ++i) {
            if (i > 0)
                stream << ", ";
            stream << m_pSignature->operandtype(i) << ' ' << argPlaceHolder(i);
        }
        stream << ')';
        if (m_pSignature->has_varyingouputsemantic())
            stream << " : " << m_pSignature->varyingouputsemantic();
    }

    void formatBody(string &body) const {
        for (int i = 0; i < m_pSignature->operandtype_size(); ++i)
            replaceAll(body, placeHolder('@', i), argPlaceHolder(i));
        for (int i = 0; i < m_pSignature->parametertype_size(); ++i)
            replaceAll(body, placeHolder('$', i), m_Instance.parametername(i));
        //        replaceAll(body, "\t", "");
        //        replaceAll(body, "\n", "");
    }

    struct CallGraphVariableFinder {
        const FunctionInstance_CallGraph &m_Callgraph;
        typedef map<int, size_t> CountMap;
        CountMap indexCount;
        vector<int> variableIndices;
        CallGraphVariableFinder(const FunctionInstance_CallGraph &callgraph) :
            m_Callgraph(callgraph) {
            const int returnValueIndex = callgraph.has_returncallindex() ? callgraph.returncallindex() : callgraph.call_size() - 1;
            if (returnValueIndex < 0 || returnValueIndex >= callgraph.call_size()) {
                ostringstream msg;
                msg << "invalid callgraph" << endl;
                msg << callgraph.DebugString() << endl;
                throw runtime_error(msg.str());
            }
            browse(returnValueIndex);
        }

        void browse(int index) {
            const size_t currentCount = ++indexCount[index];
            if (currentCount == 1) {
                const FunctionInstance_CallGraph_FunctionCall &call = m_Callgraph.call(index);
                for (int i = 0; i < call.operand_size(); ++i)
                    browse(call.operand(i));
            }
            if (indexCount[index] > 1 && notYetAdded(index))
                variableIndices.push_back(index);
        }

        bool notYetAdded(const int index) const {
            return find(variableIndices.begin(), variableIndices.end(), index) == variableIndices.end();
        }

        bool isVariable(int index) const {
            const CountMap::const_iterator itr = indexCount.find(index);
            if (itr == indexCount.end())
                return false;
            return itr->second > 1;
        }

        string getFunctionCall(const IFunctionGetter& functionGetter, const int callIndex) const {
            return getFunctionCall(functionGetter, callIndex, callIndex);
        }

        string getFunctionCall(const IFunctionGetter& functionGetter, const int callIndex, const int root) const {
            if (callIndex != root && isVariable(callIndex))
                return varPlaceHolder(callIndex);

            const FunctionInstance_CallGraph_FunctionCall &call = m_Callgraph.call(callIndex);
            const FunctionInstance &instance = call.function();
            if (instance.has_inlinedvalue())
                return instance.inlinedvalue();
            string fCall = functionGetter.getId(instance);
            fCall += '(';
            for (int i = 0; i < call.operand_size(); ++i) {
                if (i != 0)
                    fCall += ", ";
                fCall += getFunctionCall(functionGetter, call.operand(i), root);
            }
            fCall += ')';
            return fCall;
        }
    };

    string getBody(const IFunctionGetter& functionGetter) const {
        if (m_Instance.has_callgraph()) {
            ostringstream body;
            const FunctionInstance_CallGraph &callgraph = m_Instance.callgraph();
            CallGraphVariableFinder f(callgraph);
            for (vector<int>::const_iterator itr = f.variableIndices.begin(); itr != f.variableIndices.end(); ++itr) {
                const int index = *itr;
                const FunctionInstance_CallGraph_FunctionCall &call = callgraph.call(index);
                body << "const " << functionGetter.getReturnType(call.function());
                body << ' ' << varPlaceHolder(index) << " = ";
                body << f.getFunctionCall(functionGetter, index);
                body << ';' << endl;
            }
            body << "return ";
            body << f.getFunctionCall(functionGetter, callgraph.call_size() - 1);
            body << ';';
            return body.str();
        } else {
            return m_pPrototype->body();
        }
    }

    void writeBody(ostream &stream, string body) const {
        stream << " {" << endl;
        formatBody(body);
        stream << body << endl;
        stream << '}' << endl;
    }
public:
    Function(const string &id, const FunctionInstance &def, const FunctionPrototype *pPrototype, const FunctionSignature *pSignature) :
        m_Id(id), //
                m_Instance(def), //
                m_pPrototype(pPrototype), //
                m_pSignature(pSignature) {
    }

    inline const string& id() const {
        return m_Id;
    }

    inline bool inlined() const {
        return m_Instance.has_inlinedvalue();
    }

    inline const FunctionInstance& instance() const {
        return m_Instance;
    }

    inline const FunctionSignature& signature() const {
        assert(m_pSignature);
        return *m_pSignature;
    }

    void declare(ostream &stream, const IFunctionGetter& functionGetter) const {
        if (inlined())
            return;
        prependStrings(stream);
        prependParameters(stream);
        writeSignature(stream);
        writeBody(stream, getBody(functionGetter));
    }
};

static inline const FunctionPrototype& getPrototype(const FunctionInstance &def, const PrototypeFactory& factory) {
    assert(def.has_useprototype() || def.has_useprototypenamed());
    return def.has_useprototype() ? def.useprototype() : factory.getPrototype(def.useprototypenamed());
}

static string getId(const PrototypeFactory& factory, const FunctionInstance &def, const FunctionPrototype *&pPrototype, const FunctionSignature *&pSignature) {
    pSignature = NULL;
    pPrototype = NULL;
    string name;
    string idToHash;
    idToHash.reserve(2048);
    const bool hasSignature = !def.has_inlinedvalue();
    if (hasSignature) {
        const bool hasPrototype = def.has_useprototype() || def.has_useprototypenamed();
        if (hasPrototype) {
            pPrototype = &getPrototype(def, factory);
            pSignature = &pPrototype->signature();
        } else {
            pSignature = &def.callgraph().signature();
        }
        name = pSignature->name();
        assert(pSignature->parametertype_size()==def.parametername_size());
        for (int i = 0; i < pSignature->parametertype_size(); i++) {
            idToHash += pSignature->parametertype(i);
            idToHash += def.parametername(i);
        }
    } else {
        idToHash = def.inlinedvalue();
    }
    ostringstream stream;
    stream << name;
    if (!idToHash.empty())
        stream << '_' << hex << tr1::hash<string>()(idToHash);
    return stream.str();
}

class Compiler : public IFunctionGetter {
    ostringstream m_Code;
    const Program &m_Program;
    const PrototypeFactory& m_Factory;
    typedef boost::ptr_vector<Function> PtrVector;
    PtrVector m_ProcessedFunctions;

    void buildFunctionSet() {
        for (int i = 0; i < m_Program.function_size(); ++i)
            appendInstance(m_Program.function(i));
    }

    void appendInstance(const FunctionInstance &instance) {
        const FunctionSignature *pSignature = NULL;
        const FunctionPrototype *pPrototype = NULL;
        const string id = shader_factory::getId(m_Factory, instance, pPrototype, pSignature);
        if (alreadyProcessed(id))
            return;
        if (instance.has_callgraph()) {
            const FunctionInstance_CallGraph &callGraph = instance.callgraph();
            for (int i = 0; i < callGraph.call_size(); ++i)
                appendInstance(callGraph.call(i).function());
        }
        m_ProcessedFunctions.push_back(new Function(id, instance, pPrototype, pSignature));
        m_ProcessedFunctions.back().declare(m_Code, *this);
    }

    bool alreadyProcessed(const string& id) const {
        for (PtrVector::const_iterator itr = m_ProcessedFunctions.begin(); itr != m_ProcessedFunctions.end(); ++itr)
            if (itr->id() == id)
                return true;
        return false;
    }

public:
    Compiler(const Program &program, const PrototypeFactory& factory) :
        m_Program(program), m_Factory(factory) {
        buildFunctionSet();
    }

    virtual const string& getId(const FunctionInstance& instance) const {
        return get(instance).id();
    }
    virtual const string& getReturnType(const FunctionInstance& instance) const {
        return get(instance).signature().returntype();
    }

    const Function& get(const FunctionInstance& instance) const {
        const FunctionSignature *pSignature = NULL;
        const FunctionPrototype *pPrototype = NULL;
        const string id = shader_factory::getId(m_Factory, instance, pPrototype, pSignature);
        for (PtrVector::const_iterator itr = m_ProcessedFunctions.begin(); itr != m_ProcessedFunctions.end(); ++itr)
            if (itr->id() == id)
                return *itr;
        throw runtime_error("cannot get instance");
    }

    string getCode() const {
        return m_Code.str();
    }
};

string compile(const ::duke::protocol::Program &program, const PrototypeFactory& factory) {
    return Compiler(program, factory).getCode();
}

} // namespace shader_factory
