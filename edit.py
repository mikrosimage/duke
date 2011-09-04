#!/usr/bin/python
import sys, os, string, shutil, time, re

def walktree(top = '.', depthfirst = True):
	import os, stat, types
	names = os.listdir(top)
	if not depthfirst:
		yield top, names
	for name in names:
		try:
			st = os.lstat(os.path.join(top, name))
		except os.error:
			continue
		if stat.S_ISDIR(st.st_mode):
			for (newtop, children) in walktree (os.path.join(top, name), depthfirst):
				yield newtop, children
	if depthfirst:
		yield top, names

def adaptTemplate(filepath):
	fSource = open( filepath, 'r' )
	source = fSource.read( )
	fSource.close( )
	source = source.replace('$PROJECTNAME$', project_name)
	source = source.replace('$PROJECTNICKNAME$', project_nick)
	source = source.replace('$PROJECTDESCRIPTION$', project_description)
	source = source.replace('$PROJECTVERSIONNUMBER$', project_version)
	source = source.replace('$PROJECTREPOSITORY$', project_repository)
	source = source.replace('$PROJECTAUTHORS$', project_authors)
	source = source.replace('$PROJECTMAILINGLIST$', project_mailinglist)
	source = source.replace('$PROJECTWEBSITE$', project_website)
	
	# removes comment blocksw
	reg_exp = re.compile(r'\$COMMENTBLOCK\$.*\$COMMENTBLOCK\$', re.MULTILINE|re.DOTALL)
	source = re.sub(reg_exp,'',source)

	fDest = open( filepath, 'w' )
	fDest.write(source)
	fDest.close()

if __name__ == '__main__':
	try:

		project_name = ''
		while not project_name or project_name.find( ' ' ) != -1 :
			project_name = raw_input( 'Project name (without space)?: ' )

		project_nick = raw_input( 'Project short name (default "'+project_name.lower()+'") ?: ' )
		if not project_nick:
			project_nick = project_name
		project_nick = project_nick.lower()
		
		project_description = raw_input( 'Project description (default "Project under early development.") ?: ' )
		if not project_description:
			project_description = 'Project under early development.'
		
		project_version = raw_input( 'Project version number? (default "1.0.0"): ' )
		if not project_version:
			project_version = '1.0.0'

		project_repository = ''
		while not project_repository or project_repository.find( ' ' ) != -1 :
			project_repository = raw_input( 'Project repository name (http://github.com/[NAME]) ?: ' )

		project_authors = ''
		while not project_authors :
			project_authors = raw_input( 'Project authors ?: ' )

		project_mailinglist = raw_input( 'Project mailing list (default "NA") ?: ' )
		if not project_mailinglist:
			project_mailinglist = 'NA'

		project_website = raw_input( 'Project web site (default "NA") ?: ' )
		if not project_website:
			project_website = 'NA'

		print 'Processing files...'
		
		for top, names in walktree():
			for name in names:
				if top.count(".git") != 0:
					continue
				if top.count(".settings") != 0:
					continue
				# Rename templates files with a correct filename
				dName = name.replace( '_ProjectName_', project_name )
				if dName[-1] == '_':
					dName = dName[:-1]
				inputFile = os.path.join(top, name)
				outputFile = os.path.join(top, dName)
				if dName != name:
					os.rename( inputFile, outputFile )
				# Check if we need to replace things on the file
				if os.path.isfile(outputFile):
					if not any( dName.endswith(ext) for ext in ['.png', '.svg', '.py']):
						print 'Processing: ' + name + ' to ' + dName
						adaptTemplate( outputFile )

	except ValueError, v:
		try:
			( code, message ) = v
		except:
			code = 0
			message = v
		print 'Sorry, ' + str( message ) + ' (' + str( code ) + ')'
		print
		sys.exit(1)
