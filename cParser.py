






import subprocess
import sys
import os
import argparse
import tempfile
import re
from typing import List, Tuple, Optional, Set

STRUCTURE_TAGS = {"if":    ("beginif",     "endif"),
	"for":     ("beginfor",    "endfor"),
	"while":   ("beginwhile",  "endwhile"),
	"switch":  ("beginswitch","endswitch"),
	"function":("beginfunc",   "endfunc"),
	"class":("beginclass",   "endclass")}
config = """
	Language: Cpp
	BasedOnStyle: Google
	IndentWidth: 4
	AccessModifierOffset: -4
	AllowShortIfStatementsOnASingleLine: false
	AllowShortLoopsOnASingleLine: false
	AllowShortFunctionsOnASingleLine: None
	BreakBeforeBraces: Allman
	ColumnLimit: 100
	PointerAlignment: Left
	SortIncludes: false
	ReflowComments: false
	UseTab: Never
	SpacesBeforeTrailingComments: 1
	"""
	

def create_clang_config():
	"""Create a temporary clang-format configuration file for Arduino code."""
	fd, path = tempfile.mkstemp(prefix='.clang-format-', text=True)
	with os.fdopen(fd, 'w') as f:
	
		f.write(config)
		
	return path
	
def indent_else_blocks(code: str) -> str:
	"""
	After formatting, indent 'else'/'else if' lines one level deeper
	under the preceding block's closing brace.
	"""
	lines = code.split('\n')
	new_lines = []
	for line in lines:
		stripped = line.lstrip()
		if stripped.startswith('else') and new_lines:
		
			prev = new_lines[-1]
			prev_indent = len(prev) - len(prev.lstrip())
			new_indent = prev_indent + 4
			new_lines.append(' ' * new_indent + stripped)
		else:
			new_lines.append(line)
			
		
	
	return '\n'.join(new_lines)
	
def format_code(code: str) -> str:
	"""
	Format Arduino code using clang-format, then indent else-if blocks.
	Args:
	code: The original code
	Returns:
	Formatted code with else-if indented
	"""
	try:
	
		
		
		
		clang_format_path = r"C:\\Users\\lopezl10\\AppData\\Roaming\\Python\\Python312\\Scripts\\clang-format"
		clang_yaml_path = r"C:\\Users\\lopezl10\\AppData\\Local\\RedHorseVR\\C2VFC_parser\\vfc.yaml"
		stream = os.popen(f'"{clang_format_path}" -style=file:"{clang_yaml_path}" "{code_file}"')
		
		formatted = stream.read()
		print( "--->" + code_file )
		print( "output: [ " ,  formatted + "]" )
		return formatted
	except Exception as e:
		print(f"Error during formatting: {e}", file=sys.stderr)
		sys.exit(1)
		
	
def add_structure_comments(code: str) -> str:
	"""
	Add structure comments focusing only on selected block types, handling if-else chains.
	Args:
	code: The formatted code
	Returns:
	Code with added structure comments
	"""
	lines = code.split('\n')
	result = lines.copy()
	function_regex = re.compile(r'^\s*(?!(?:if|for|while|else|switch)\b)[\w\s\*\&\:\<\>\~]+\w+\s*\([^;{]*\)\s*$')
	blocks: List[Tuple[str, int, Optional[int], Optional[int]]] = []
	brace_stack: List[Tuple[int, int, Optional[int]]] = []
	tagged_lines: Set[int] = set()
	
	for i, line in enumerate(lines):
		stripped = line.strip()
		if not stripped or stripped.startswith('//'):
		
			continue
			
		indent = len(line) - len(line.lstrip())
		
		if re.search(r'^\s*if\s*\(', line):
		
			blocks.append(('if', i, None, None))
		elif re.search(r'^\s*for\s*\(', line):
			blocks.append(('for', i, None, None))
		elif re.search(r'^\s*while\s*\(', line):
			blocks.append(('while', i, None, None))
		elif re.search(r'^\s*switch\s*\(', line):
			blocks.append(('switch', i, None, None))
		elif re.search(r'^\s*class\s*\(', line):
			blocks.append(('class', i, None, None))
		elif function_regex.match(line):
			blocks.append(('function', i, None, None))
			
			
			
			
			
		
		if '{' in stripped:
		
			
			
			potential_owners = [(idx, block) for idx, block in enumerate(blocks)
			if block[2] is None and block[1] <= i and i - block[1] <= 2]
			if potential_owners:
			
				
				bidx = potential_owners[-1][0]
				btype, sline, _, eline = blocks[bidx]
				blocks[bidx] = (btype, sline, i, eline)
			else:
				bidx = None
				
			brace_stack.append((i, indent, bidx))
			
		
		if '}' in stripped:
		
			if brace_stack:
			
				_, _, bidx = brace_stack.pop()
				if bidx is not None:
				
					btype, sline, ob, _ = blocks[bidx]
					blocks[bidx] = (btype, sline, ob, i)
					
				
			
		
	
	
	updated = []
	for btype, sline, ob, eline in blocks:
		if btype == 'if' and eline is not None:
		
			chain_end = eline
			j = eline + 1
			while j < len(lines):
				st = lines[j].strip()
				if not st or st.startswith('//'):
				
					j += 1
					continue
					
				if st.startswith('else'):
				
					# locate '{'
					if '{' in st:
					
						open_j = j
					else:
						k = j + 1
						while k < len(lines) and '{' not in lines[k]:
							k += 1
							
						
						open_j = k
						
					count = 0
					for ch in lines[open_j]:
						if ch == '{':
						
							count += 1
							
						if ch == '}':
						
							count -= 1
							
						
					
					m = open_j + 1
					while m < len(lines) and count > 0:
						for ch in lines[m]:
							if ch == '{':
							
								count += 1
								
							if ch == '}':
							
								count -= 1
								
							
						
						m += 1
						
					
					chain_end = m - 1
					j = chain_end + 1
					continue
					
				break
				
			
			updated.append((btype, sline, ob, chain_end))
		else:
			updated.append((btype, sline, ob, eline))
			
		
	
	blocks = updated
	
	for btype, sline, ob, eline in blocks:
		if btype in STRUCTURE_TAGS and sline is not None and eline is not None:
		
			start_tag, end_tag = STRUCTURE_TAGS[btype]
			if sline not in tagged_lines and '//' not in result[sline]:
			
				result[sline] = f"{result[sline]} //{start_tag}"
				tagged_lines.add(sline)
				
			if eline not in tagged_lines and '//' not in result[eline]:
			
				result[eline] = f"{result[eline]} //{end_tag}"
				tagged_lines.add(eline)
				
			
		
	
	return '\n'.join(result)
	
def process_file(input_file: str, output_file: str = None, skip_format: bool = False) -> None:
	
	
	global code_file
	code_file = input_file
	try:
	
		code = open(input_file).read()
		formatted = code if skip_format else format_code(code)
		final = add_structure_comments(formatted)
		if output_file:
		
			open(output_file, 'w').write(final)
			print(f"Output written to {output_file}")
		else:
			print(final)
			
	except Exception as e:
		print(f"Error: {e}", file=sys.stderr)
		sys.exit(1)
		
	return final
	
############################################ HUMAN ONLY CAN MODIFY BELOW
VFCSEPERATOR = ';//'
Begins = [
	"beginfunc",
	"beginmethod",
	"beginclass",
	"beginif",
	"begintry",
	"beginswitch",
	"beginwith",
	"beginwhile",
	"beginfor",
	]
Ends = [
	"endfunc",
	"endmethod",
	"endclass",
	"endif",
	"endtry",
	"endswitch",
	"endwith",
	"endfor",
	"endwhile",
	]
begin_type = {
	"beginfunc": "input",
	"beginmethod": "input",
	"beginclass": "input",
	"beginif": "branch",
	"begintry": "branch",
	"beginswitch": "branch",
	"beginwith": "branch",
	"beginwhile": "loop",
	"beginfor": "loop",
	}
end_type = {
	"endfunc": "end",
	"endmethod": "end",
	"endclass": "end",
	"endif": "bend",
	"endtry": "bend",
	"endswitch": "bend",
	"endwith": "bend",
	"endfor": "lend",
	"endwhile": "lend",
	}
paths = [
	"else if",
	"else",
	"case",
	"except",
	"finally",
	]
ends = [
	"return",
	"continue",
	"break",
	]
events = [
	"#include",
	"delay",
	]
outputs = [
	"Serial",
	"write",
	]
	

def is_path(line: str) -> bool:
	"""
	"""
	parts = line.strip().split(None, 1)
	if not parts:
	
		return False
		
	if parts[0].strip(" :") in paths:
	
		return True
		
	
def replace_string_literals(input_string):
	result = re.sub(r'(["\'])(.*?)(\1)', '0', input_string)
	return result
	
def split_on_comment(input_string):
	match = re.search(r'(?<!")#.*$', temp_str)
	if match:
	
		s1 = input_string.strip()
		s2 = match.strip()
	else:
		s1, s2 = input_string.strip(), ""
		
	return (s1, s2)
	
INLINECOMMENT = '//'
def split_string(input_string):
	temp_str = replace_string_literals(input_string)
	parts = temp_str.split( INLINECOMMENT , 1)
	s1 = input_string.strip()
	if len(parts) > 1 :
	
		s2 = parts[1]
		s1 = s1.replace( INLINECOMMENT   + s2, "")
	else:
		s2 = ""
		
	return (s1, s2)
	
def get_marker( comment ):
	parts = comment.strip().split(None, 1)
	if not parts:
	
		return "none"
		
	marker = parts[0]
	return marker
	
def first_token(code):
	tokens = re.split(r'[.;(]+', code.strip())
	return tokens[0] if tokens else "none"
	
def get_VFC_type(code : str, line: str) -> Optional[str]:
	
	
	
	
	token = code.strip().split(None, 1)[0] if len(code) > 1 else "none"
	if first_token(code) in outputs:
	
		return "output"
	elif first_token(code) in ends:
		return "end"
	elif token in events:
		return "event"
	elif is_path(code):
		return 'path'
		
	parts = line.strip().split(None, 1)
	if not parts:
	
		return "set"
		
	marker = parts[0]
	if marker in Begins:
	
		return begin_type[marker]
		
	if marker in Ends:
	
		return end_type[marker]
		
	return "set"
	
def generate_VFC(input_string):
	strings = input_string.split("\n")
	VFC = ''
	fix_stack = []
	function_type = r'(?:void|int|float|double|char|long|short|bool|inline|static|extern|APIENTRY|\w|\*|&)*\s+\w+\s*\('
	method_type = r'\b[\w\s&\*]+::'
	prev_type = 'set'
	prev_code = ''
	for string in strings:
		if not string.strip():
		
			continue
			
		code, comment = split_string(string)
		code = code.strip()
		type = get_VFC_type(code, comment)
		#--------------------------------------------------------------------------------------------------------- FIX
		#--------------------------------------------------------------------------------------------------------- FIX
		#--------------------------------------------------------------------------------------------------------- FIX
		if   ( re.match( r'^if\b', code ) or  re.match( r'^\w* struct {', code ) ) and type != 'branch' :
		
			type = "branch"
			if not re.match( r'^if\b.*;$', code ) :
			
				fix_stack.append( 'bend' )
				
			comment = ' + br ' + comment
		elif   code == '{' and (prev_type == 'path' or 'case' in prev_code )  :
			fix_stack.append( 'end' )
			comment = ' + p{ ' + comment
		elif   '#pragma' in code  :
			type = 'event'
			comment = ' +ev ' + comment
		elif   type == 'path' and 'case' in prev_code  :
			type = 'set'
			comment = ' +cp ' + comment
		elif   re.match( r'^try\b', code ) and type == 'set' :
			type = 'branch'
			fix_stack.append( 'bend' )
			comment = ' + try ' + comment
		elif   re.match( r".*\bcase\b" , code )  :
			type = 'path'
			comment = ' + case ' + comment
		elif   re.match( r".*\bcatch\b" , code )  :
			type = 'path'
			comment = ' + cat ' + comment
		elif   re.match( r'^#if', code ) and type == 'set' :
			type = 'branch'
			comment = ' + #if ' + comment
		elif   re.match( r'^#end', code ) and type == 'set' :
			type = 'bend'
			comment = ' + #eif ' + comment
		elif   re.match( r'^return\b', code ) and type == 'set' :
			type = 'end'
			comment = ' + end ' + comment
		elif   re.match( r'} while\b', code ) and type == 'set' :
			type = "lend"
			comment = ' + dw ' + comment
		elif   re.match( r'^default\b', code )  or  re.match( r'^#else', code )   :
			type = "path"
			
			comment = ' + def ' + comment
		elif   re.match( r'\} else\b', code ) and type != 'path' :
			type = "path"
			comment = ' + pa ' + comment
		elif ( re.match( r'^while\b', code ) or re.match( r'^for\b', code ) or re.match( r'^do\b', code )  )  and type != 'loop' :
			type = "loop"
			fix_stack.append( 'lend' )
			comment = ' + lo ' + comment
		elif   re.match(function_type, code ) or re.match(method_type, code ) or re.match( r'\w*\s+APIENTRY' ,  code ) or re.match( r'^\s*class\b' ,  code ):
			type = 'input'
			if  not '}' in code  :
			
				if  not r';' in code  :
				
					fix_stack.append( 'end' )
					comment = ' + in ' + comment
				else:
					type = 'process'
					comment = ' + pr in ' + comment
					
			else:
				comment = ' + sl in ' + comment
				
		elif   re.match( r'^}', code ) and type == 'set' :
			#try-catch-exception
			try:
				type = fix_stack.pop()
				comment = ' + pop  ' + comment
			except :
				type = 'set'
				comment = ' + def pop  ' + comment
				
			
		prev_type = type
		prev_code = code
		#--------------------------------------------------------------------------------------------------- FIX
		#--------------------------------------------------------------------------------------------------- FIX
		#--------------------------------------------------------------------------------------------------- FIX
		marker = get_marker( comment )
		if marker == "endclass" :
		
			VFC += f"bend(){VFCSEPERATOR}\n"
			
		if type == "input" :
		
			pass
			
		if re.match( r'\s*class\b' ,  code ):
		
			VFC += f"end(){VFCSEPERATOR}\n"
			
		if re.match( r'\};' ,  code ):
		
			VFC += f"bend(){VFCSEPERATOR}<--- end class\n"
			type = 'end'
			
		if re.match( r'^(public|protected|private):' ,  code ) and type=='set':
		
			type = 'path'
			
		VFC += f'{type}({code}){VFCSEPERATOR} {comment}\n'
		if re.match( r'\s*class\b' ,  code ):
		
			VFC += f"branch(){VFCSEPERATOR}<---class\n"
			VFC += f"path(){VFCSEPERATOR} --- \n"
			
			
		if type == "branch":
		
			VFC += f"path(){VFCSEPERATOR}\n"
			
		if type == "branch" and re.match( r'^if\b.*;$', code ) :
		
			VFC += f"bend(){VFCSEPERATOR}\n"
			
		if marker == "beginclass" :
		
			VFC += f"branch(){VFCSEPERATOR}\n"
			VFC += f"path(){VFCSEPERATOR}\n"
			VFC += f"path(){VFCSEPERATOR}\n"
			
		
	
	return VFC
	
def  footer( exportname  ):
	ENVTOK = 'INSECTA'
	foot = f';{ENVTOK} EMBEDDED SESSION INFORMATION\n'
	foot+='; 255 16777215 65280 16777088 16711680 32896 8421504 0 255 255 16777215 4227327 2960640\n'
	foot+= f'; { os.path.basename(exportname) } // \n'
	foot+='; notepad.exe\n'
	foot+=f';{ENVTOK} EMBEDDED ALTSESSION INFORMATION\n'
	foot+='; 880 168 766 1516 0 110   392   31 C++.key  0\n'
	return foot
	
def __fix_VFC_paths( input_string ):
	strings = input_string.split("\n")
	VFC = ''
	skip_next = 0
	for i  in range( len(strings) ) :
		code = strings[i]
		if code.startswith( "branch")  :
		
			code2 = strings[i+1].strip()
			code3 = strings[i+2].strip()
			if code2.startswith("path()")   and   code3.startswith("set({)"):
			
				VFC +=code + "\n"
				VFC += "path({)" + VFCSEPERATOR  + '\n'
				skip_next = 3
				
			
		if skip_next > 0:
		
			skip_next -= 1
			continue
			
		VFC +=code + "\n"
		
	
	return VFC
	
def fix_VFC_paths( input_string ):
	strings = input_string.split("\n")
	VFC = ''
	skip_next = 0
	for i  in range( len(strings) ) :
		code = strings[i]
		if code.startswith( "branch")  :
		
			code2 = strings[i+1].strip()
			code3 = strings[i+2].strip()
			if code2.startswith("path()")   and   code3.startswith("set({)"):
			
				VFC +=code + "\n"
				VFC += "path({)" + VFCSEPERATOR  + '\n'
				skip_next = 3
				
			
		if skip_next > 0:
		
			skip_next -= 1
			continue
			
		VFC +=code + "\n"
		
	
	return VFC
	
def main():
	global modified_code
	p = argparse.ArgumentParser()
	p.add_argument('input_file')
	p.add_argument('-o', '--output')
	p.add_argument('--skip-format', action='store_true')
	args = p.parse_args()
	modified_code  = process_file(args.input_file, args.output, args.skip_format)
	VFC = generate_VFC(modified_code)
	VFC = fix_VFC_paths( VFC )
	print( modified_code )
	with open(args.input_file+'.vfc', 'w') as VFC_output:
	
		VFC_output.write(VFC)
		VFC_output.write( footer( args.input_file ) )
		
	
if __name__ == '__main__':

	main()
	

#  Export  Date: 12:02:10 PM - 14:May:2025.

