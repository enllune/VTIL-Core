// Copyright (c) 2020 Can Boluk and contributors of the VTIL Project   
// All rights reserved.   
//    
// Redistribution and use in source and binary forms, with or without   
// modification, are permitted provided that the following conditions are met: 
//    
// 1. Redistributions of source code must retain the above copyright notice,   
//    this list of conditions and the following disclaimer.   
// 2. Redistributions in binary form must reproduce the above copyright   
//    notice, this list of conditions and the following disclaimer in the   
//    documentation and/or other materials provided with the distribution.   
// 3. Neither the name of mosquitto nor the names of its   
//    contributors may be used to endorse or promote products derived from   
//    this software without specific prior written permission.   
//    
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE   
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR   
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF   
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN   
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)   
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  
// POSSIBILITY OF SUCH DAMAGE.        
//
#pragma once
#include <string>
#include <optional>
#include <array>
#include <vtil/symex>
#include <vtil/utility>

// [Configuration]
// Determine whether we should use the simplification of (A-B) as the delta 
// or the value estimated by the xpointers, and the number of xpointers we use.
//
#ifndef VTIL_SYM_PTR_SAFE_DISP
	#define VTIL_SYM_PTR_SAFE_DISP 0
	#define VTIL_SYM_PTR_XPTR_KEYS 8
#endif

namespace vtil::symbolic
{
	// Declare a symbolic pointer to be used within symbolic execution context.
	//
	struct pointer : reducable<pointer>
	{
		// Declares the symbolic pointer weak.
		//
		struct make_weak
		{
			pointer operator()( pointer p ) { return ( p.strenght = INT32_MIN, p ); }
		};

		// The symbolic expression that will represent the virtual address 
		// if resolved to an immediate value.
		// 
		boxed_expression base;
		uint64_t flags = 0;
		
		// Z-Pointer is an estimation of the actual virtual adresss 
		//
		uint64_t zpointer;
		int32_t strenght = 0;
		std::array<uint64_t, VTIL_SYM_PTR_XPTR_KEYS> xpointer;

		// Construct null pointer.
		//
		pointer() : zpointer( 0 ) { xpointer.fill( 0 ); }
		pointer( std::nullptr_t ) : pointer() {}

		// Default copy/move.
		//
		pointer( pointer&& ) = default;
		pointer( const pointer& ) = default;
		pointer& operator=( pointer&& ) = default;
		pointer& operator=( const pointer& ) = default;

		// Construct from symbolic expression.
		//
		pointer( expression&& base );
		pointer( const expression& base ) : pointer( expression{ base } ) {}

		// Simple pointer offseting.
		//
		pointer operator+( int64_t dst ) const;
		pointer operator-( int64_t dst ) const { return operator+( -dst ); }

		// Calculates the distance between two pointers as an optional constant.
		//
		std::optional<int64_t> operator-( const pointer& o ) const;

		// Conversion to human-readable format.
		//
		std::string to_string() const
		{
			return base.to_string();
		}

		// Define reduction.
		//
		REDUCE_TO( flags, strenght, xpointer, base );
	};
};