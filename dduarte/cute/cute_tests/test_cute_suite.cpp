/*********************************************************************************
 * This file is part of CUTE.
 *
 * CUTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CUTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CUTE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Peter Sommerlad
 *
 *********************************************************************************/

#include "test_cute_suite.h"
#include "cute_suite.h"
#include "cute_equals.h"

namespace {
	void atest(){}
}
void test_cute_suite(){
	cute::suite s;
	ASSERT_EQUAL(0u,s.size());
	s += CUTE(atest);
	s += CUTE(atest);
	ASSERT_EQUAL(2u,s.size());
	s += s;
	ASSERT_EQUAL(4u,s.size());
}
