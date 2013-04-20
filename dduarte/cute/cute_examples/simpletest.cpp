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
 * Copyright 2007 Peter Sommerlad
 *
 *********************************************************************************/

#include "cute.h"

int lifeTheUniverseAndEverything = 41;

void mysimpletest(){
    ASSERT(lifeTheUniverseAndEverything == 6*7);
}
#include <iostream>
#include "cute_runner.h"
int main1(){
	using namespace std;
	cute::test t=CUTE(mysimpletest);
	if (cute::runner<>()(t)){
		cout << "OK" << endl;
	} else {
		cout << "failed" << endl;
	}
	return 0;
}
#include "ide_listener.h"
int main2(){
	using namespace std;

	return cute::runner<cute::ide_listener>()(CUTE(mysimpletest));
}


#include "cute_test.h"
#include "cute_equals.h"
int anothertest(){
	ASSERT_EQUAL(42,lifeTheUniverseAndEverything);
	return 0;
}

cute::test tests[]={
#ifdef __GNUG__
	CUTE(mysimpletest)
	,mysimpletest
	,CUTE(anothertest)
#else /* for MSVC... */
	CUTE(mysimpletest)
	,CUTE(mysimpletest)
	,CUTE(reinterpret_cast<void(*)()>(anothertest))
#endif
};

struct ATestFunctor {
	void operator()(){
		ASSERT_EQUAL_DELTA(42.0,static_cast<double>(lifeTheUniverseAndEverything),0.001);
	}
};
#include "cute_suite.h"
int main3(){
	using namespace std;

	cute::runner<cute::ide_listener> run;
	cute::suite s(tests,tests+(sizeof(tests)/sizeof(tests[0])));
	s+=ATestFunctor();
	return run(s,"suite");
}

int main(){
	main1();
	main2();
	main3();
}
