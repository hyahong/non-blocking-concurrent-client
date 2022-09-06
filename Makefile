CC			= g++
CXXFLAGS	= #-Wall -Wextra -Werror
RM			= rm -rf

INCS		= incs
SRCS		= srcs/main.cpp \
			  srcs/Connection/Connection.cpp srcs/Connection/ClientRequest.cpp srcs/Connection/ClientResponse.cpp \
			  srcs/File/Cluster.cpp srcs/File/FileBlock.cpp 
LIBS		= -lssl -lcrypto

OBJS		= $(patsubst %.cpp,%.o,$(SRCS))

NAME		= client

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(NAME) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -I$(INCS) -c $< -o $@

re: fclean all

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)
