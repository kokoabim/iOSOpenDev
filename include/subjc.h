/*
 
 subjc.h
 Subjective-C: An Objective-C message logger.
 Copyright (C) 2009  KennyTM~
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 
 */

#ifndef SUBJC_HOOK
#define SUBJC_HOOK

#if !__arm__ && !__i386__
#error Only available on x86 and ARM platforms!
#endif

#if __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <objc/objc.h>

	void SubjC_initialize ();
	
	enum SubjC_FilterType {
		SubjC_Deny,
		SubjC_Allow
	};
	
	void SubjC_clear_filters();
	void SubjC_filter_method(enum SubjC_FilterType blacklist, Class class_, SEL selector);
	void SubjC_filter_class(enum SubjC_FilterType blacklist, Class class_);
	void SubjC_filter_selector(enum SubjC_FilterType blacklist, SEL selector);
	void SubjC_default_filter_type(enum SubjC_FilterType blacklist);
	void SubjC_filter_class_prefixes(enum SubjC_FilterType blacklist, unsigned prefixes_count, const char* const prefixes[]);
	void SubjC_filter_class_prefix(enum SubjC_FilterType blacklist, const char* prefix);
	
	void SubjC_set_file(FILE* f);
	void SubjC_set_maximum_depth(size_t maximum_depth);
	void SubjC_set_print_arguments(bool print_arguments);
	void SubjC_set_print_return_value(bool print_return_value);
	void SubjC_set_print_timestamp(bool print_timestamp);
	
	void SubjC_start();
	void SubjC_end();

#if __cplusplus
}
#endif

#endif
