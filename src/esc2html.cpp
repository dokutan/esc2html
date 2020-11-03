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

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>

// for popen() and pclose()
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>

#include "esc2html.h"

int main( int argc, char* argv[] ){
	
	// parse arguments
	bool flag_sixel = false, flag_help = false, flag_no_header = false;
	bool flag_quiet = false, flag_font = false, flag_title = false;
	std::string arg_font, arg_title;
	
	int opt;
	while( (opt = getopt(argc, argv, "shpqf:t:")) != -1 ){
		switch( opt ){
			case 's': flag_sixel = true; break;
			case 'h': flag_help = true; break;
			case 'p': flag_no_header = true; break;
			case 'q': flag_quiet = true; break;
			case 'f': flag_font = true; arg_font = optarg; break;
			case 't': flag_title = true; arg_title = optarg; break;
		}
	}
	
	if( flag_help ){
		std::cout << help_message;
		return 0;
	}
	
	// html header
	if( !flag_no_header ){
		std::string modified_header = html_header;
		if( flag_title )
			modified_header = std::regex_replace( modified_header, std::regex("esc2html"), arg_title );
		std::cout << modified_header;
	}
	
	// <pre> element
	std::string modified_pre = html_pre;
	if( flag_font )
		modified_pre = std::regex_replace( modified_pre, std::regex("monospace"), arg_font );
	std::cout << modified_pre;
	
	
	// read complete stdin into buffer
	std::string line, data = "";
	while( std::getline( std::cin, line ) ){
		data += line;
		data += "\n";
	}
	
	// tag stack
	std::vector< html_tag > tag_stack;
	
	// calculate color lookup table
	calculate_colors_216();
	
	// process data
	for( size_t i = 0; i < data.size(); ){
		
		std::string byte = data.substr( i, 1 );
		
		// replace < > & etc.
		if( replacements.find(byte) != replacements.end() ){
			std::cout << replacements.at(byte);
			i++;
		}
		
		// escape sequence
		else if( byte == "\e" ){
			
			// CSI (Control Sequence Introducer)
			if( data.substr( i, 2 ) == "\e[" ){
				
				// get full sequence
				std::string sequence = "\e[";
				i+=2;
				do{
					sequence += data.at( i );
					i++;
				}while( sequence.back() < '\x40' || sequence.back() > '\x7e' );
				
				// decode sequence
				decode_csi( std::cout, tag_stack, sequence, flag_quiet );
			}
			
			// Sixel graphics
			else if( data.substr( i, 3 ) == "\ePq" ){
				
				// get full sequence
				std::string sequence = "\ePq";
				i+=3;
				
				while( 1 ){
					sequence += data.at( i );
					i++;
					
					// end of the sequence indicated by ST (\e\\) 
					if( data.at( i-1 ) == '\\' && data.at( i-2 ) == '\e' )
						break;
				}
				
				if( flag_sixel )
					decode_sixel( std::cout, sequence );
			}
			
			// hyperlink (OSC 8 = \e]8)
			else if( data.substr( i, 3 ) == "\e]8" ){
				
				// skip beginning of sequence
				i+=3;
				
				// get URI
				std::string uri;
				while( 1 ){
					uri += data.at( i );
					i++;
					
					// end of the sequence indicated by ST (\e\\) or BEL (\x07)
					if( (data.at( i-1 ) == '\\' && data.at( i-2 ) == '\e') || data.at( i-1 ) == '\x07' )
						break;
				}
				uri = std::regex_replace( uri, std::regex( "(;.*;|\e\\\\|\x07)" ), "" );
				
				if( uri != "" )
					apply_tag( std::cout, tag_stack, html_tag( "a", "href", uri, "a", "href", ".*" ) );
				
				else
					apply_tag( std::cout, tag_stack, html_tag( "", "a" ) );
				
			}
			
			// other sequences terminated by ST, skips the whole sequence
			else if( std::regex_match( data.substr( i, 2 ), std::regex( "\e[P\\]X_^]" ) ) ){
				
				while( 1 ){
					i++;
					
					// end of the sequence indicated by ST (\e\\) or BEL (\x07)
					if( (data.at( i-1 ) == '\\' && data.at( i-2 ) == '\e') || data.at( i-1 ) == '\x07' )
						break;
				}
				
			}
			
			// unknown sequence "\e[0x20-0x2f]*[0x30-0x7e]"
			else{
				
				do{
					i++;
				}while( data.at( i ) < '\x30' || data.at( i ) > '\x7e' );
				i++;
				
			}
			
		}
		
		// normal character
		else{
			std::cout << byte;
			i++;
		}
		
	}
	
	// close all tags
	std::for_each( tag_stack.rbegin(), tag_stack.rend(), [&](auto& t){
		t.print_close( std::cout );
	} );
	
	// clear the stack
	tag_stack.clear();
	
	// html footer
	std::cout << "\n</pre>";
	if( !flag_no_header )
		std::cout << html_footer;
	
	return 0;
} 
