/*-------------------------------------------------------Inclusions and Macros-------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/
#define SWFCHECK_VER_MAJ 0
#define SWFCHECK_VER_MIN 1

#include<stdio.h>	// Standard Input-Output Library
#include<stdlib.h>	// Memory Management and the rest
#include<limits.h>	// Standard Limits

#include<libswftag/swftag.h>	// Project Header
#include<libswftag/swfmath.h>	// For some minor calculations and for some macros needed for compile time evaluation
#include<libswftag/error.h>	// Error codes and handling
#include<libswftag/check_functions.h>	// Validity checkers for each individual tag
#include<libswftag/decompression.h>	// Decompression (not really rn lol)

#define N_MGMT_FLAGS 8	// Number of Management flags in program state
#define UINT_BIT sizeof(unsigned int) * CHAR_BIT // Number of bits in an unsigned integer (same as the number of bits in a signed integer)

#define FATAL_ERR_MSG "\x1b[31;1;4;5;7m" "FATAL ERROR:" "\x1b[0m" " "
#define WARN_MSG "\x1b[35;1m" "WARNING:" "\x1b[0m" " "

/*--------------------------------------------------------------Structs--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

struct program_state
{
	unsigned int mgmt_flags[M_CEILDIV(N_MGMT_FLAGS, UINT_BIT)];

	FILE *ifile;
	char *ifile_n;

	FILE *ofile;
	char *ofile_n;

	FILE *logfile;
	char *logfile_n;
};
typedef struct program_state pstate;

/*------------------------------------------------------------Static Data------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

const static char *help_fmt = FM_RESET FM_BOLD "SWFcheck version %u.%u" FM_RESET "\nAn SWF movie verification utility\n\n\t" FM_BOLD "Usage" FM_RESET ":\n\t\tswfcheck [options] input_file(Default: stdin)\n\n\t" FM_BOLD "Options" FM_RESET ":\n\t\t-h : Print this help and exit\n";

/*--------------------------------------------------------------Program--------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

err close_file(pdata *state, FILE *file)
{
	if(!state || !file)
	{
		err ret = error_handler(state, EFN_ARGS);
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	if(fclose(file))
	{
		return error_handler(state, EFL_CLOSE);
	}
	return ALL_CLEAR;
}

err open_file(pdata *state, FILE **file, char *file_n, char *opt)
{
	if(!state || !file_n)
	{
		err ret = error_handler(state, EFN_ARGS);
		if(ER_ERROR(ret))
		{
			return ret;
		}
	}
	*file = fopen(file_n, opt);
	if(!(*file))
	{
		return error_handler(state, EFL_OPEN);
	}
	return ALL_CLEAR;
}

void print_summary(pdata *state)
{
	if(state->pec_list == NULL)
	{
		printf(COL_GR FM_BOLD "ALL CLEAR, check passed with no peculiarities encountered" FM_RESET "\n");
	}
	printf("Compression-type: %c, version: %ju, movie-size: %ju\nMovie-rect:\n\tfield-size: %ju, xmin: %ju, xmax: %ju, ymin: %ju, ymax: %ju (All in twips)\nTotal number of tags: %ju\n", state->compression, (uintmax_t)(state->version), (uintmax_t)(state->movie_size), (uintmax_t)(state->header.movie_rect.field_size), (uintmax_t)(state->header.movie_rect.fields[0]), (uintmax_t)state->header.movie_rect.fields[1], (uintmax_t)(state->header.movie_rect.fields[2]), (uintmax_t)(state->header.movie_rect.fields[3]), (uintmax_t)(state->n_tags));
}

void swfcheck_file(pstate *prog, pdata *state, char *name)
{
	prog->mgmt_flags[0] |= 0x1;
	if(name)
	{
		prog->ifile_n = name;
		if(prog->ifile != stdin && prog->ifile != stdout && prog->ifile != stderr)
		{
			close_file(state, prog->ifile);
		}
		open_file(state, &(prog->ifile), prog->ifile_n, "rb");
	}
	else
	{
		prog->ifile_n = "stdin";
		prog->ifile = stdin;
	}
	printf(FM_BOLD FM_UNLN "File: %s" FM_RESET "\n", prog->ifile_n);
	init_parse_data(state);
	check_file_validity(state, prog->ifile);
	close_file(state, prog->ifile);
	print_summary(state);
}


int main(int nargs, char *args[])
{
	struct program_state state;

	size_t flag_ints = M_CEILDIV(N_MGMT_FLAGS, UINT_BIT);
	for(size_t i=0; i<flag_ints; i++)
	{
		state.mgmt_flags[i] = 0;
	}

	state.ifile = stdin;
	state.ofile = stdout;
	state.logfile = stderr;

	state.ifile_n = "stdin";
	state.ofile_n = "stdout";
	state.logfile_n = state.ofile_n;

	size_t argcnt = nargs;
	argcnt--;

	pdata check_state;

	while(argcnt)
	{
		if(args[nargs-argcnt][0] == '-')	// Flag checking
		{
			if(args[nargs-argcnt][1] == '\0')
			{
				swfcheck_file(&state, &check_state, NULL);
				if(argcnt != 1)
				{
					destroy_parse_data(&check_state);
					puts("\n");
				}
			}
			size_t itr = 1;
			while(args[nargs-argcnt][itr])
			{
				switch(args[nargs-argcnt][itr])
				{
					case '-':
						break;
					case 'h':
						fprintf(stdout, help_fmt, SWFCHECK_VER_MAJ, SWFCHECK_VER_MIN);
						exit(0x0);
					default:
						fprintf(stderr, FATAL_ERR_MSG "Invalid flag supplied :%c\nUse " FM_BOLD "swftag -h" FM_RESET " to print the help\n", args[nargs-argcnt][itr]);
						exit(0x1);
						next_arg_read:
						fprintf(stderr, WARN_MSG "Already have used a file specifier N.A.(no file specifiers implemented, how'd you even get this message? lolol) in the current options chain. Ignoring...\n");
				}
			}
		}
		else
		{
			swfcheck_file(&state, &check_state, args[nargs-argcnt]);
			if(argcnt != 1)
			{
				destroy_parse_data(&check_state);
				puts("\n");
			}
		}
		argcnt--;
	}

	exit(0x0);
}
