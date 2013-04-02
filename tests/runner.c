#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include <string.h>
#include <stdio.h>


/* auto generated by Makefile */
#include "../test_catalog.inc"

#include "litmus.h"

int run_test(struct testcase *tc) {
	int status;
	pid_t pid;

	printf("** Testing: %s... ", tc->description);
	fflush(stdout);
	SYSCALL( pid = fork() );
	if (pid == 0) {
		/* child: init liblitmus and carry out test */
		SYSCALL( init_litmus() );
		tc->function();
		exit(0);
	} else {
		/* parent: wait for completion of test */
		SYSCALL( waitpid(pid, &status, 0) );
		if (WEXITSTATUS(status) == 0)
			printf("ok.\n");
	}
	return WEXITSTATUS(status) == 0;
}

int run_tests(int* testidx, int num_tests, const char* plugin)
{
	int idx, i;
	int ok = 0;

	printf("** Running tests for %s.\n", plugin);
	for (i = 0; i < num_tests; i++) {
		idx = testidx[i];
		ok += run_test(test_catalog + idx);
	}
	return ok;
}

#define streq(s1, s2) (!strcmp(s1, s2))

int main(int argc, char** argv)
{
	int ok, i;

	printf("** LITMUS^RT test suite.\n");

	if (argc == 2) {
		for (i = 0; i < NUM_PLUGINS; i++)
			if (streq(testsuite[i].plugin, argv[1])) {
				ok = run_tests(testsuite[i].testcases,
					       testsuite[i].num_cases,
					       testsuite[i].plugin);
				printf("** Result: %d ok, %d failed.\n",
				       ok, testsuite[i].num_cases - ok);
				return ok == testsuite[i].num_cases ? 0 : 3;
			}
		fprintf(stderr, "** Unknown plugin: '%s'\n", argv[1]);
		return 1;
	} else {
		fprintf(stderr, "Usage: %s <plugin name>\n", argv[0]);
		fprintf(stderr, "Supported plugins: ");
		for (i = 0; i < NUM_PLUGINS; i++)
			fprintf(stderr, "%s ", testsuite[i].plugin);
		fprintf(stderr, "\n");
		return 2;
	}
}
