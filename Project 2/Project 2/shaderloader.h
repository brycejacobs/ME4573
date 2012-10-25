#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <stdio.h>
#include <stdlib.h>


class ShaderLoader {
	private:



	public:

		char *load(const char *name) {
			FILE *fp = 0;
			void *p = 0;
			size_t n = 0;
			if ((fp = fopen(name, "rb"))) // Open the file.
			{
				if (fseek(fp, 0, SEEK_END) == 0) // Seek the end.
				if ((n = (size_t) ftell(fp))) // Tell the length.
				if (fseek(fp, 0, SEEK_SET) == 0) // Seek the beginning.
				if ((p = calloc(n + 1, 1))) // Allocate a buffer.
				fread(p, 1, n, fp); // Read the data.
				fclose(fp); // Close the file.
			}
			char *pointer;

			return pointer = (char*) p;
				





		}








};



















#endif 