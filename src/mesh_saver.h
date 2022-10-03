#ifndef __MESH_SAVER_H_
#define __MESH_SAVER_H_

#include <stdio.h>
#include <vector>
#include "cgut.h"
#include "cgmath.h"

bool cg_save_mesh(const char* vpath, const char* ipath, std::vector<vertex> vertices, std::vector<uint> indices)
{
	FILE* savevFile = fopen(vpath, "wb");
	FILE* saveiFile = fopen(ipath, "wb");
	size_t retval1, retval2;
	if (savevFile == NULL)
	{
		printf("vertex file open fail\n");
		return false;
	}
	if (saveiFile == NULL)
	{
		printf("index file open fail\n");
		fclose(savevFile);
		return false;
	}

	retval1 = fwrite((void *)&vertices[0], sizeof(vertex), vertices.size(), savevFile);
	retval2 = fwrite((void*)&indices[0], sizeof(uint), indices.size(), saveiFile);

	if (retval1 != vertices.size())
	{
		printf("writing vertex buffer failed\n");
		fclose(savevFile);
		fclose(saveiFile);
		return false;
	}
	if (retval2 != indices.size())
	{
		printf("writing index buffer failed\n");
		fclose(savevFile);
		fclose(saveiFile);
		return false;
	}

	fclose(savevFile);
	fclose(saveiFile);
	return true;
}

#endif