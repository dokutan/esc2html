/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

class html_tag {
	
	public:
		
		// tag data
		std::string name;
		std::map< std::string, std::string > attributes;
		
		// opening this tag should close these tags if they are on the stack
		// tag name → attributes
		std::map< std::string, std::map< std::string, std::regex > > closes;
		
		html_tag(){
			
		}
		
		html_tag( std::string tag_name ){
			name = tag_name;
			closes.emplace( tag_name, std::map< std::string, std::regex >({}) );
		}
		
		html_tag( std::string tag_name, std::string closes_name ){
			name = tag_name;
			closes.emplace( closes_name, std::map< std::string, std::regex >({}) );
		}
		
		html_tag( std::string tag_name, std::string attribute_name, std::string attribute_value ){
			name = tag_name;
			attributes.emplace( attribute_name, attribute_value );
			closes.emplace( tag_name, std::map< std::string, std::regex >({ {attribute_name, std::regex(attribute_value)} }) );
		}
		
		html_tag( std::string tag_name, std::string closes_name, std::string closes_attribute_name, std::string closes_attribute_value ){
			name = tag_name;
			closes.emplace( closes_name, std::map< std::string, std::regex >({ {closes_attribute_name, std::regex(closes_attribute_value)} }) );
		}
		
		html_tag( std::string tag_name, std::string attribute_name, std::string attribute_value,
				std::string closes_name, std::string closes_attribute_name, std::string closes_attribute_value ){
			
			name = tag_name;
			attributes.emplace( attribute_name, attribute_value );
			closes.emplace( closes_name, std::map< std::string, std::regex >({ {closes_attribute_name, std::regex(closes_attribute_value)} }) );
		}
		
		// print tag
		void print_open( std::ostream& output ){
			
			if( name != "" ){
				output << "<" << name;
				for( auto& a :attributes ){
					output << " " << a.first << "=\"" << a.second << "\"";
				}
				output << ">";
			}
			
		}
		void print_close( std::ostream& output ){
			if( name != "" )
				output << "</" << name << ">";
		}
		
		bool closed_by( html_tag& new_tag ){
	
			if( new_tag.closes.contains( name ) ){
				
				if( new_tag.closes.at( name ).size() != 0 ){
				
					for( auto& a : new_tag.closes.at( name ) ){
						
						if( attributes.contains( a.first ) ){
							if( std::regex_match( attributes.at( a.first ), a.second ) )
								return true;
						}
					}
					
					return false;
				}
				
				return true;
			}
			
			return false;	
		}
};

// global constants
const std::string html_header = 
R"(<!DOCTYPE html>
<html lang="">
<head>
	<title>esc2html</title>
	<style>
		@keyframes blinking {
			0%  { opacity:1; }
			50% { opacity:0; }
		}
		#blink-slow {
			animation: blinking;
			animation-duration: 0.6s;
			animation-iteration-count: infinite;
			animation-timing-function: step-start;
		}
		#blink-rapid {
			animation: blinking;
			animation-duration: 0.3s;
			animation-iteration-count: infinite;
			animation-timing-function: step-start;
		}
	</style>
</head>
<body>
)";
const std::string html_pre = "<pre style=\"font-family: monospace\">\n";
const std::string html_footer = "\n</body>\n</html>\n";

const std::string foreground_color_default = "#000000";
const std::string background_color_default = "#ffffff";

// characters that need escaping in html
const std::map< std::string, std::string > replacements = {
	{ "\r", "" },
	//{ "\n", "<br>" },
	{ "&", "&amp;" },
	{ "<", "&lt;" },
	{ ">", "&gt;" },
	{ "'", "&apos;" },
	{ "\"", "&quot;" }
};
 
const std::map< std::string, html_tag > format_html = {
	{ "1",  html_tag( "b" ) }, // bold
	{ "3",  html_tag( "i" ) }, // italic
	{ "4",  html_tag( "u" ) }, // underline
	{ "5",  html_tag( "span", "id", "blink-slow", "span", "id", "blink.*" ) }, // slow blink
	{ "6",  html_tag( "span", "id", "blink-rapid", "span", "id", "blink.*" ) }, // rapid blink
	{ "8",  html_tag( "span", "style", "opacity:0" ) }, // conceal
	{ "9",  html_tag( "s" ) }, // strike through
	{ "21", html_tag( "u", "style", "text-decoration-style:double" ) }, // double underline
	{ "22", html_tag( "", "b" ) }, // bold off
	{ "23", html_tag( "", "i" ) }, // italic off
	{ "24", html_tag( "", "u" ) }, // underline off
	{ "25", html_tag( "", "span", "id", "blink.*" ) }, // blink off
	{ "28", html_tag( "", "span", "style", "opacity:.*" ) }, // conceal off
	{ "29", html_tag( "", "s" ) }, // strike through off
	{ "39", html_tag( "", "span", "style", "color:.*" ) }, // text color off
	{ "49", html_tag( "", "span", "style", "background-color:.*" ) }, // bg color off
	{ "53", html_tag( "span", "style", "text-decoration:overline" ) }, // overline
	{ "55", html_tag( "", "span", "style", "text-decoration:overline" ) }, // overline off
	{ "73", html_tag( "sup", "sub" ) }, // superscript
	{ "74", html_tag( "sub", "sup" ) } // subscript
};

const std::map< std::string, std::string > sgr_skip = {
	{ "10", "default font" },
	{ "11", "font 1" },
	{ "12", "font 2" },
	{ "13", "font 3" },
	{ "14", "font 4" },
	{ "15", "font 5" },
	{ "16", "font 6" },
	{ "17", "font 7" },
	{ "18", "font 8" },
	{ "19", "font 9" },
	{ "20", "fraktur" },
	{ "26", "proportional spacing" },
	{ "50", "disable proportional spacing" },
	{ "51", "framed" },
	{ "52", "encircled" },
	{ "54", "framed and encircled off" },
	{ "60", "ideogram underline" },
	{ "61", "ideogram double underline" },
	{ "62", "ideogram overline" },
	{ "63", "ideogram double overline" },
	{ "64", "ideogram stress marking" },
	{ "65", "ideogram attributes off" }
};

// these are the color values for the VGA palette
const std::map< std::string, std::string > colors_fg = {
	{ "30", "color:rgb(0, 0, 0)" },
	{ "31", "color:rgb(170, 0, 0)" },
	{ "32", "color:rgb(0, 170, 0)" },
	{ "33", "color:rgb(170, 85, 0)" },
	{ "34", "color:rgb(0, 0, 170)" },
	{ "35", "color:rgb(170, 0, 170)" },
	{ "36", "color:rgb(0, 170, 170)" },
	{ "37", "color:rgb(170, 170, 170)" },
	{ "90", "color:rgb(85, 85, 85)" },
	{ "91", "color:rgb(255, 85, 85)" },
	{ "92", "color:rgb(85, 255, 85)" },
	{ "93", "color:rgb(255, 255, 85)" },
	{ "94", "color:rgb(85, 85, 255)" },
	{ "95", "color:rgb(255, 85, 255)" },
	{ "96", "color:rgb(85, 255, 255)" },
	{ "97", "color:rgb(255, 255, 255)" }
};
const std::map< std::string, std::string > colors_bg = {
	{ "40", "background-color:rgb(0, 0, 0)" },
	{ "41", "background-color:rgb(170, 0, 0)" },
	{ "42", "background-color:rgb(0, 170, 0)" },
	{ "43", "background-color:rgb(170, 85, 0)" },
	{ "44", "background-color:rgb(0, 0, 170)" },
	{ "45", "background-color:rgb(170, 0, 170)" },
	{ "46", "background-color:rgb(0, 170, 170)" },
	{ "47", "background-color:rgb(170, 170, 170)" },
	{ "100", "background-color:rgb(85, 85, 85)" },
	{ "101", "background-color:rgb(255, 85, 85)" },
	{ "102", "background-color:rgb(85, 255, 85)" },
	{ "103", "background-color:rgb(255, 255, 85)" },
	{ "104", "background-color:rgb(85, 85, 255)" },
	{ "105", "background-color:rgb(255, 85, 255)" },
	{ "106", "background-color:rgb(85, 255, 255)" },
	{ "107", "background-color:rgb(255, 255, 255)" }
};
const std::vector< std::string > colors_216_levels = { "00", "5f", "87", "af", "d7", "ff" };
std::map< int, std::string > colors_216;
const std::map< int, std::string > colors_gray = {
	{ 232, "#080808" },
	{ 233, "#121212" },
	{ 234, "#1c1c1c" },
	{ 235, "#262626" },
	{ 236, "#303030" },
	{ 237, "#3a3a3a" },
	{ 238, "#444444" },
	{ 239, "#4e4e4e" },
	{ 240, "#585858" },
	{ 241, "#626262" },
	{ 242, "#6c6c6c" },
	{ 243, "#767676" },
	{ 244, "#808080" },
	{ 245, "#8a8a8a" },
	{ 246, "#949494" },
	{ 247, "#9e9e9e" },
	{ 248, "#a8a8a8" },
	{ 249, "#b2b2b2" },
	{ 250, "#bcbcbc" },
	{ 251, "#c6c6c6" },
	{ 252, "#d0d0d0" },
	{ 253, "#dadada" },
	{ 254, "#e4e4e4" },
	{ 255, "#eeeeee" }
};

void calculate_colors_216(){
	
	for( int r = 0; r < 6; r++ ){
		for( int g = 0; g < 6; g++ ){
			for( int b = 0; b < 6; b++ ){
				colors_216.emplace( (16+(36*r)+(6*g)+b), "#"+colors_216_levels.at(r)+colors_216_levels.at(g)+colors_216_levels.at(b) );
			}
		}
	}
}

const std::string help_message = 
R"(esc2html - convert escape sequences to html

Usage:
... | esc2html [options] > output.html

Options:
-h	show this message
-q	don't show warnings about unknown and skipped sequences
-s	convert sixel graphics with sixel2png(1) and base64(1)
-p	print only the main <pre> element, no html header and footer
-t arg	set the title of the html document, defauĺt is esc2html 
-f arg	set the font of the html document, default is monospace
)";

// global variables
bool inverted_colors = false;
std::vector< std::string > last_fg_color = { "30" };
std::vector< std::string > last_bg_color = { "107" };

// turns a foreground color command into a background color command and vice versa
// returns other commands unchanged
std::string invert_color( std::string command ){
	if( std::regex_match( command, std::regex( "3." ) ) )
		command[0] = '4';
	else if( std::regex_match( command, std::regex( "4." ) ) )
		command[0] = '3';
	else if( std::regex_match( command, std::regex( "9[0-7]" ) ) )
		command = std::regex_replace( command, std::regex( "9" ), "10" );
	else if( std::regex_match( command, std::regex( "10[0-7]" ) ) )
		command = std::regex_replace( command, std::regex( "10" ), "9" );
		
	return command;
}

void apply_tag( std::ostream& output, std::vector< html_tag >& tag_stack, html_tag tag ){
	
	std::vector< html_tag > temp_stack; // holds tags that need to be closed and reopened
	size_t skip = 0; // number of tags to skip from the bottom of the stack
	
	// count tags that don't need to be closed
	for( auto it = tag_stack.begin(); it != tag_stack.end(); it++ ){
		if( it->closed_by( tag ) )
			break;
		else
			skip++;
	}
	
	// close opened tags
	std::for_each( tag_stack.rbegin(), tag_stack.rend()-skip, [&](auto& t){
		t.print_close( output );
		if( !t.closed_by( tag ) )
			temp_stack.push_back( t );
	} );
	
	// re-open tags
	std::for_each( temp_stack.rbegin(), temp_stack.rend(), [&](auto& t){
		t.print_open( output );
	} );
	
	// delete closed tags
	std::erase_if( tag_stack, [&](auto& t){
		return t.closed_by( tag );
	} );
	
	// open new tag
	tag_stack.push_back( tag );
	tag_stack.back().print_open( output );
			
}

// turns a CSI sequence into html tags
void decode_csi( std::ostream& output, std::vector< html_tag >& tag_stack, std::string sequence, bool quiet ){
	
	// check if sequence is an SGR sequence
	if( !std::regex_match( sequence, std::regex( "\e\\[.*m" ) ) )
		return;
	
	// split sequence into individual commands
	std::vector< std::string > commands;
	std::regex r( "\\d+" );
	std::smatch m;
	while( regex_search( sequence, m, r ) ){
		commands.push_back( m.str() );
		sequence = m.suffix();
	}
	
	// no commands: "\e[m" is equivalent to "\e[0m" (reset everything)
	if( commands.size() == 0 )
		commands.push_back( "0" );
	
	// decode commands
	for( size_t i = 0; i < commands.size(); ){
		
		std::string command = commands.at( i );
		i++;
		
		// invert colors: substitute command with color commands
		if( command == "7" && !inverted_colors ){
			inverted_colors = true;
			
			commands.insert( commands.begin()+i, last_bg_color.begin(), last_bg_color.end() );
			commands.insert( commands.begin()+i, last_fg_color.begin(), last_fg_color.end() );
			continue;
			
		} else if( command == "27" && inverted_colors ){
			inverted_colors = false;
			
			std::vector< std::string > fg_color = last_bg_color;
			std::vector< std::string > bg_color = last_fg_color;
			fg_color[0] = invert_color( fg_color[0] );
			bg_color[0] = invert_color( bg_color[0] );
			
			commands.insert( commands.begin()+i, bg_color.begin(), bg_color.end() );
			commands.insert( commands.begin()+i, fg_color.begin(), fg_color.end() );
			continue;
		}
		
		// if in inverted colors mode: modify color commands
		if( inverted_colors )
			command = invert_color( command );
		
		// formatting
		html_tag tag;
		bool apply_formatting = false;
		
		// turn everything off
		if( command == "0" ){
			
			std::set< std::string > keep_open = { "a" }; // don't close these tags
			std::vector< html_tag > temp_stack; // holds tags that need to be closed and reopened
			size_t skip = 0; // number of tags to skip from the bottom of the stack
			
			// count tags that don't need to be closed
			for( auto it = tag_stack.begin(); it != tag_stack.end(); it++ ){
				if( !keep_open.contains( it->name ) )
					break;
				else
					skip++;
			}
			
			// close opened tags
			std::for_each( tag_stack.rbegin(), tag_stack.rend()-skip, [&](auto& t){
				t.print_close( output );
				if( keep_open.contains( t.name ) )
					temp_stack.push_back( t );
			} );
			
			// re-open tags
			std::for_each( temp_stack.rbegin(), temp_stack.rend(), [&](auto& t){
				t.print_open( output );
			} );
			
			// delete closed tags
			std::erase_if( tag_stack, [&](auto& t){
				return !keep_open.contains( t.name );
			} );
			
			last_fg_color = { "30" };
			last_bg_color = { "107" };
			inverted_colors = false;
			
			continue;
		}
		
		// skip commands
		else if( sgr_skip.find( command ) != sgr_skip.end() && !quiet )
			std::cerr << "Warning: skipping SGR parameter " << command << " (" << sgr_skip.at( command ) << ")\n";
		
		// formatting
		else if( format_html.find( command ) != format_html.end() ){
			tag = format_html.at( command );
			apply_formatting = true;
		}
		
		// 3/4 bit text color
		else if( colors_fg.find( command ) != colors_fg.end() ){
			tag.name = "span";
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("color:.*")} }) );
			tag.attributes.emplace( "style", colors_fg.at( command ) );
			apply_formatting = true;
			last_fg_color = { command };
		}
		
		// 3/4 bit background color
		else if( colors_bg.find( command ) != colors_bg.end() ){
			tag.name = "span";
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("background-color:.*")} }) );
			tag.attributes.emplace( "style", colors_bg.at( command ) );
			apply_formatting = true;
			last_bg_color = { command };
		}
		
		// 8 bit text color
		else if( command == "38" && commands.at( i ) == "5" ){
			
			int color = std::stoi( commands.at( i+1 ) );
			std::string attribute;
			tag.name = "span";
			
			if( color >= 0 && color <= 7 )
				attribute = colors_bg.at( std::to_string( color+30 ) );
			else if( color >= 8 && color <= 15 )
				attribute = colors_bg.at( std::to_string( color+82 ) );
			else if( color >= 16 && color <= 231 )
				attribute = "color:" + colors_216.at( color );
			else if( color >= 232 && color <= 255 )
				attribute = "color:" + colors_gray.at( color );
			
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("color:.*")} }) );
			tag.attributes.emplace( "style", attribute );
			apply_formatting = true;
			last_fg_color = { "38", "5", commands.at( i+1 ) };
			i += 2;
		}
		
		// 8 bit background color
		else if( command == "48" && commands.at( i ) == "5" ){
			
			int color = std::stoi( commands.at( i+1 ) );
			std::string attribute;
			tag.name = "span";
			
			if( color >= 0 && color <= 7 )
				attribute = colors_bg.at( std::to_string( color+40 ) );
			else if( color >= 8 && color <= 15 )
				attribute = colors_bg.at( std::to_string( color+92 ) );
			else if( color >= 16 && color <= 231 )
				attribute = "background-color:" + colors_216.at( color );
			else if( color >= 232 && color <= 255 )
				attribute = "background-color:" + colors_gray.at( color );
			
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("background-color:.*")} }) );
			tag.attributes.emplace( "style", attribute );
			apply_formatting = true;
			last_bg_color = { "48", "5", commands.at( i+1 ) };
			i += 2;
		}
		
		// 24 bit text color
		else if( command == "38" && commands.at( i ) == "2" ){
			
			tag.name = "span";
			tag.attributes.emplace( "style", "color:rgb("+commands.at( i+1 )+", "+commands.at( i+2 )+", "+commands.at( i+3 )+")" );
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("color:.*")} }) );
			apply_formatting = true;
			last_fg_color = { "38", "2", commands.at( i+1 ), commands.at( i+2 ), commands.at( i+3 ) };
			i += 4;
		}
		
		// 24 bit background color
		else if( command == "48" && commands.at( i ) == "2" ){
			
			tag.name = "span";
			tag.attributes.emplace( "style", "background-color:rgb("+commands.at( i+1 )+", "+commands.at( i+2 )+", "+commands.at( i+3 )+")" );
			tag.closes.emplace( "span", std::map< std::string, std::regex >({ {"style", std::regex("background-color:.*")} }) );
			apply_formatting = true;
			last_bg_color = { "48", "2", commands.at( i+1 ), commands.at( i+2 ), commands.at( i+3 ) };
			i += 4;
		}
		
		if( apply_formatting )
			apply_tag( output, tag_stack, tag );
		
	}
	
}

// Turns a sixel image into a base64 encoded png using sixel2png/convert and base64
void decode_sixel( std::ostream& output, std::string sequence ){
	
	// create temporary file
	char filename[] = "/tmp/esc2htmlXXXXXX";
	int outfile = mkstemp( filename );
	if( outfile == -1 )
		return;
	
	// write sixel data to file
	write( outfile, sequence.c_str(), sizeof(char)*sequence.size() );
	close( outfile );
	
	// call sixel2png/convert TODO! make the conversion program configurable by the user
	//FILE* sixel_to_base64 = popen( std::string("sixel2png -i "+std::string(filename)+" | base64").c_str(), "r" );
	FILE* sixel_to_base64 = popen( std::string("convert sixel:"+std::string(filename)+" png:- | base64").c_str(), "r" );
	if( sixel_to_base64 == NULL )
		return;
	
	// get result
	output << "<div><img src=\"data:image/png;base64,";
	char buffer[32];
	while( fgets( buffer, sizeof(buffer), sixel_to_base64 ) != NULL )
		output << buffer;
	output << "\" /></div>";
	
	// cleanup
	pclose( sixel_to_base64 );
	remove( filename );
}
