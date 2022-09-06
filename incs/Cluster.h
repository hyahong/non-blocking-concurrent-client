#ifndef _CLUSTER_H_
# define _CLUSTER_H_

# include "FileBlock.h"

class Cluster
{
public:
	/* coplien */
	Cluster();
	Cluster(const Cluster &c);
	virtual ~Cluster();

	Cluster &operator=(const Cluster &c);

	void Download(std::string url, std::string path);

private:
};

#endif
