#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

typedef struct{
	char* op_code;
	int firstParam:
	int secondParam:
} __attribute__((packed))
t_serializable;

#endif /* SERIALIZABLE_H_ */
