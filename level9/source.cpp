#include <stdlib.h>
#include <string.h>

class N {
	private:
		// The size may be wrong here but the offset before memcpy's
		// destination is 0x4 from "this" and "value" is at 0x68
		char	annotation[0x64];
		int		value;

	public:
		N(int new_value) {
			value = new_value;
		}

		int operator+(N *right) {
			return value + right->value;
		}

		int operator-(N *right) {
			return value - right->value;
		}

		void setAnnotation(char *new_annotation) {
			memcpy(annotation, new_annotation, strlen(new_annotation));
		}
};

int	main(int argc, char **argv)
{
	N *a;
	N *b;

	if (argc < 2)
		exit(1);
	a = new N(5);
	b = new N(6);
	a->setAnnotation(argv[1]);
	return (b->operator+(a));
}
