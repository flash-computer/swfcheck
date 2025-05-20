#include<libswftag/swftag.h>
#include<libswftag/error.h>

#include<stdlib.h>
#include<stdio.h>

/*-----------------------------------------------------------Error Strings-----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

/* DO NOT USE. DOES NOT WORK RIGHT NOW LMAO */
/* It honestly brings me immense shame that I do not know how to make multidimensional arrays of strings in c */

/* This is ugly with lots of repetition and wastage, but I kept it this way so that we have a simple static logic to actually print these things. RIP memory */
#define FATAL_ERR_MSG "\x1b[31;1;4;5;7m" "FATAL ERROR:" "\x1b[0m\a" " "
#define WARN_MSG "\x1b[35;1m" "WARNING:" "\x1b[0m\a" " "
#define PECULIARITY_MSG "\x1b[34;1;4;7m" "PECULIARITY:" "\x1b[0m" " "
#define ALL_CLEAR_MSG COL_GR FM_BOLD "ALL CLEAR:" FM_RESET " "

#define UNKNOWN_ERR_MSG "Unknown Error"

#define undefined_categories_messages {UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
swf_signature_err_msg
swf_shortfile_err_msg
swf_tag_err_msg
swf_improper_err_msg
*/
#define swf_error_messages {UNKNOWN_ERR_MSG, "Invalid file signature", "Unexpected end of file", "File is improper", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
prog_misc_err_msg
prog_arguments_err_msg
*/
#define prog_error_messages {"Misc Program Error, likely due to the use of an unimplemented feature", "Invalid Argument received", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
file_misc_err_msg
file_open_err_msg
file_close_err_msg
file_read_err_msg
file_write_err_msg
file_seek_err_msg
*/
#define file_error_messages {"Undefined File related error", "Could not read from file", "Could not close file", "Requested read failed", "Requested write failed", "Requested seek failed. Ensure the file is not a stream like standard input.", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
memory_allocfailure_err_msg
*/
#define memory_error_messages {UNKNOWN_ERR_MSG, "Memory allocation faliure", UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG, UNKNOWN_ERR_MSG}

/*
all_clear_msg
*/
#define UNKNOWN_PECULIAR_EXIT_MSG "Verification failed with unknown but present caveats"
#define peculiar_exit_messages {"This is not an error. You should never see this",  "Peculiarity Filtered", "Premature End", UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG, UNKNOWN_PECULIAR_EXIT_MSG}

const static char error_messages[16][16][100] = {peculiar_exit_messages, undefined_categories_messages, memory_error_messages, undefined_categories_messages, file_error_messages, undefined_categories_messages, prog_error_messages, undefined_categories_messages, swf_error_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages, undefined_categories_messages};

#define UNKNOWN_PECULIARITY_MSG "This peculiarity has not been defined yet. If you encounter this, something is wrong."
#define N_PEC ((PEC_MAX - PEC_MIN) + 1)
#define PECULIAR_STRING_MESSAGES {"Padding in a bitfield isn't 0", "Tag is larger than it should be", "Mythical tag with no standard definition encountered", "Feature or Tag encountered in swf newer than the reported swf version", "Actual file size smaller than reported in header", "Undefined tag encountered", "Swf ends without a properly placed T_END tag", "Anomalous swf version", "File extends after the movie", "Reserved bit tampered", "Width of a bitfield is too big",  "Short tag used for a tag that is usually long tag exclusive"}

const static char peculiar_messages[N_PEC][100] = PECULIAR_STRING_MESSAGES;

#define N_LOW_RISK_INVAL_TAGS 4

// Tag 700 is used often by flv swfs exported from Techsmith Camtasia Studio
// Tag 777 Seems to be a side effect of using swftools, especially merge-tool
// Tag 255 Often contains a single byte, 0x20 or 0x30 and I don't know for sure what it does, besides the fact that AS2 flashes have the byte as 0x20 and AS3 ones have 0x30.
// Tag 253 is a product of an old obfuscation trick by embedding actions within some amount of junk
// Additional Checks can be added for both to ensure that Tag 700 only contains the Camtasia Studio Watermark and that Tag 777 is empty
const static ui16 low_risk_inval_tags[N_LOW_RISK_INVAL_TAGS] = {700, 777, 255, 253};

/*----------------------------------------------------------Implementations----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

void print_summary(pdata *state);

err error_handler(pdata *state, err code)
{
	if(!ER_ERROR(code))
	{
		fprintf(stderr, ALL_CLEAR_MSG "No errors encountered\n");
		return code;
	}
	fprintf(stderr, "Error code: %x\n", code);
	fprintf(stderr, "Pdata state: state->u_movie: %p, state->tag_stream_end: %p\nTags scanned: %ju\n", (void *)state->u_movie, (void *)state->tag_stream_end, (uintmax_t)state->n_tags);
	if(state->tag_stream_end)
	{
		swf_tag *tag = ((dnode *)state->tag_stream_end)->data;
		fprintf(stderr, "Problematic tag: %d, name: %s, data_pointer: %p, offset = %tu\n", tag->tag, tag_name(tag->tag), tag->tag_data, ((tag->tag_data) - (state->u_movie)));
	}
	fprintf(stderr, "%s%s\n", (code & 0xF0)?FATAL_ERR_MSG:WARN_MSG, error_messages[(code & 0xF0)>>4][code & 0xF]);
	puts("\nSummary:\n");
	printf("Compression-type: %c, version: %ju, movie-size: %ju\nMovie-rect:\n\tfield-size: %ju, xmin: %ju, xmax: %ju, ymin: %ju, ymax: %ju (All in twips)\nTotal number of tags: %ju\n", state->compression, (uintmax_t)(state->version), (uintmax_t)(state->movie_size), (uintmax_t)(state->header.movie_rect.field_size), (uintmax_t)(state->header.movie_rect.fields[0]), (uintmax_t)state->header.movie_rect.fields[1], (uintmax_t)(state->header.movie_rect.fields[2]), (uintmax_t)(state->header.movie_rect.fields[3]), (uintmax_t)(state->n_tags));
	exit(0x10 + ((code>>4)&0xF));
}

err callback_peculiarity(pdata *state, dnode *node)
{
	if(!state || !node)
	{
		return error_handler(state, EFN_ARGS);
	}
	ui32 pattern = ((peculiar *)(node->data))->pattern;
	fprintf(stderr, PECULIARITY_MSG "Peculiarity encountered: 0x%jx\n", (uintmax_t)pattern);
	if(pattern == PEC_INVAL_TAG || pattern == PEC_TIME_TRAVEL || pattern == PEC_MYTHICAL_TAG)
	{
		if(!(state->tag_stream_end))
		{
			return error_handler(state, EFN_ARGS);
		}
		swf_tag *last_tag = ((dnode *)(state->tag_stream_end))->data;
		if(!last_tag)
		{
			return error_handler(state, EFN_ARGS);
		}
		fprintf(stderr, FM_BOLD "Tag code: %ju, Tag size: %ju, tag_number:%ju" FM_RESET "\n", (uintmax_t)last_tag->tag, (uintmax_t)last_tag->size, (uintmax_t)state->n_tags);
	}
	if(pattern >= PEC_MIN && pattern <= PEC_MAX)
	{
		fprintf(stdout, FM_BOLD "%s" FM_RESET "\n", peculiar_messages[pattern-PEC_MIN]);
		switch(pattern)
		{
			case PEC_INVAL_TAG:
				if(!(state->tag_stream_end))
				{
					return error_handler(state, EFN_ARGS);
				}
				swf_tag *last_tag = ((dnode *)(state->tag_stream_end))->data;
				if(!last_tag)
				{
					return error_handler(state, EFN_ARGS);
				}
				for(size_t i=0; i<N_LOW_RISK_INVAL_TAGS; i++)
				{
					if(last_tag->tag == low_risk_inval_tags[i])
					{
						goto tag_accepted;
					}
				}
				return error_handler(state, WAF_PEC_FILTERED);
				tag_accepted:
				break;
			case PEC_DATA_AFTER_MOVIE:
				// TODO: Add an option to write the truncated, valid movie to a new file
				break;
		}
	}
	else
	{
		fprintf(stderr, COL_RD FM_INVR "\a" UNKNOWN_PECULIARITY_MSG FM_RESET "\n");
		return error_handler(state, WAF_PEC_FILTERED);
	}
	return 0;
}
