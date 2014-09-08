/* 
 * Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <iostream>
#include "dist.h"

int copy_data(struct archive *arch, struct archive *archive_write)
{
	int r;
	const void *buff;
	size_t size;
	int64_t offset;

	for (;;) {
		r = archive_read_data_block(arch, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return r;
		r = archive_write_data_block(archive_write, buff, size, offset);
		if (r < ARCHIVE_OK) {
			printf("%s\n", archive_error_string(archive_write));
			return r;
		}
	}
}

void extract(const char *proj_path)
{
	struct archive *arch;
	struct archive *ext;
	struct archive_entry *ent;
	int r;

	arch = archive_read_new();
	archive_read_support_format_all(arch);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, 0);
	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_filename(arch, proj_path, 10240)))
		return;
	for (;;) {
		r = archive_read_next_header(arch, &ent);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			printf("%s\n", archive_error_string(arch));
		if (r < ARCHIVE_WARN)
			return;
		r = archive_write_header(ext, ent);
		if (r < ARCHIVE_OK)
			printf("%s\n", archive_error_string(ext));
		else if (archive_entry_size(ent) > 0) {
			r = copy_data(arch, ext);
			if (r < ARCHIVE_OK)
				printf("%s\n", archive_error_string(ext));
			if (r < ARCHIVE_WARN)
				return;
		}
		r = archive_write_finish_entry(ext);
		if (r < ARCHIVE_OK)
			printf("%s\n", archive_error_string(ext));
		if (r < ARCHIVE_WARN)
			return;
	}
	archive_read_close(arch);
	archive_read_free(arch);
	archive_write_close(ext);
	archive_write_free(ext);
	return;
}
