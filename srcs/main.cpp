#include <iostream>
#include "Connection.h"

int		main(void)
{
	Connection conn;

	conn.SetURL("https://d1f0m5hlxy4kyb.cloudfront.net/test.txt");
	conn.SetMethod(Method::GET);
	//conn.Connect();
	return 0;
}
