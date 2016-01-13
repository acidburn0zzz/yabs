// Copyright (c) 2013-2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

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
