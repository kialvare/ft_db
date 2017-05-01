/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcheung <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/18 15:31:05 by kcheung           #+#    #+#             */
/*   Updated: 2017/04/29 15:42:57 by kcheung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_db.h"
/******************************************************/
/**              funtion prototypes                  **/
/******************************************************/
int argc_len(char **argv);
int open_record(char *filename);
int open_record_delete(char *filename);
void close_record(int fd);
off_t find_record(int fd, char *key);
int insert_record(int fd, person_rec *rec);
int *select_record(char **col, t_table *table, char *Constraint);
int delete_record(int fd, char *key);
t_col *new_header(char *name);
t_col *insrt_header(t_col *head, char *name);
t_tree *new_datanode(int row, char *value);
t_tree *insrt_datanode(t_col *col, t_tree *head, int row, char *val);
t_table *build_table(int fd);
void print_table(t_table table);
int handle_insert(int argc, char **argv, person_rec *rec);
int handle_delete(int argc, char **argv);
char*scan_for_file(char **argv);
char **parse_col_list(char **argv);
int convert_to_index(int **colist_index, char **col_list, t_table *table);
int is_selected_column(int index, int *indexlist, int col_count);
void print_record(int fd, int *row_list, char **col_list, t_table table);
char *get_constraint(char **argv);
int handle_select(int argc, char **argv);
int sql(char *op, char **argv, char *line);
char **lsh_split_line(char *line, const char *delim);
int sql_execute(char **args, char *line);
char *lsh_read_line(void);
char **parse_values(char **values);
void free_rec(person_rec *rec);

/* lseek(fd, 0, SEEK_SET);   #<{(| seek to start of file |)}># */
/* lseek(fd, 100, SEEK_SET); #<{(| seek to offset 100 from the start |)}># */
/* lseek(fd, 0, SEEK_END);   #<{(| seek to end of file (i.e. immediately after the last byte) |)}># */
/* lseek(fd, -1, SEEK_END);  #<{(| seek to the last byte of the file |)}># */
/* lseek(fd, -10, SEEK_CUR); #<{(| seek 10 bytes back from your current position in the file |)}># */
/* lseek(fd, 10, SEEK_CUR);  #<{(| seek 10 bytes ahead of your current position in the file |)}># */
/*  */

/* Helper Functions */
int		argc_len(char **argv)
{
	char	**temp;
	int		count;

	temp = argv;
	count = 0;
	while (*temp)
	{
		count++;
		temp++;
	}
	return (count);
}

/* Database Commands */
int		open_record(char *filename)
{
	int fd;

	fd = open(filename, O_RDWR | O_APPEND, 0644);
	if (fd == -1)
		perror("open_record");
	return fd;
}

int		open_record_delete(char *filename)
{
	int fd;

	fd = open(filename, O_RDWR , 0644);
	if (fd == -1)
	{
		perror("Table Does not Exist");
		exit(-1);
	}
	return fd;
}

void	close_record(int fd)
{
	close(fd);
}

/* find_record(int fd, char *key): */
/* Reads from fd. If record with key is found the position of the beginning is returned. */
/* If record is not found, -1 is returned. */

off_t	find_record(int fd, char *key)
{
	printf("searching record...\n");
	off_t			pos;
	char			*line;
	char			**array;
	long long int	bytes_read;
	int				ret;

	pos = lseek(fd, 0, SEEK_SET);
	while ((ret = get_next_line(fd, &line)) != -1)
	{
		bytes_read = ft_strlen(line);
		array = lsh_split_line(line, ",");
		if (ret == 0)
			return (-1);
		else if (!ft_strcmp(key, array[0]))
		{
			pos = lseek(fd, 0, SEEK_CUR);
			pos = lseek(fd, pos - bytes_read - 1, SEEK_SET);
			return (pos);
		}
		pos = lseek(fd, 0, SEEK_CUR);
	}
	return (-1);
}

int		insert_record(int fd, person_rec *rec)
{
	int		ret;

	if (find_record(fd, rec->key) != -1)
	{
		printf("Record Already Exists\n");
		return (0);
	}
	ret = 0;
	printf("inserting...\n");
	ret += write(fd, rec->key, ft_strlen(rec->key));
	ret += write(fd, ",", 1);
	ret += write(fd, rec->fname, ft_strlen(rec->fname));
	ret += write(fd, ",", 1);
	ret += write(fd, rec->lname, ft_strlen(rec->lname));
	ret += write(fd, ",", 1);
	ret += write(fd, rec->age, ft_strlen(rec->age));
	ret += write(fd, "\n", 1); 
	close(fd);
	return (ret);
}

/* Should Return a list of Row # that satisfy Constraints
 * If Constrain is NULL all row numbers with data is returned */
int		*select_record(char **col, t_table *table, char *Constraint)
{
	int		*rows;
	t_tree	*temp;
	t_col	*temp_col;
	int		i;
	char	**array;
	/* char	op; */

	/* valid_col(col, table); */
	i = 0;
	rows = (int *)malloc(sizeof(int) * table->header->data_count + 1);
	if (Constraint)
	{
		temp_col = table->header;
		/* if(ft_strchr(Constraint, '=')) */ 						// Not yet implemented: for comparing constraints
		/* 	op = '='; */
		/* if(ft_strchr(Constraint, '>')) */
		/* 	op = '>'; */
		/* if(ft_strchr(Constraint, '<')) */
		/* 	op = '<'; */
		array = lsh_split_line(Constraint, "=><");
		while (ft_strcmp(temp_col->name, array[0]))
			temp_col = temp_col->next;
		temp = temp_col->data;
		while (temp)
		{
			if (!ft_strcmp((*temp).value, array[1]))
			{
				rows[i] = (*temp).row;
				i++;
			}
			temp = temp->next;
		}
		rows[i] = 0;
	}
	else
	{
		temp = table->header->data;
		while (temp)
		{
			rows[i] = (*temp).row;
			temp = temp->next;
			i++;
		}
		rows[i] = 0;
	}
	(void)col;
	return (rows);
}

int		delete_record(int fd, char *key)
{
	printf("deleting...\n");
	int		ret;
	char	*line;
	char	**array;
	int		delete_bytes;
	off_t	pos;
	
	pos = lseek(fd, 0, SEEK_SET);								//resets the fd position
	while ((ret = get_next_line(fd, &line)) != -1)
	{
		delete_bytes = ft_strlen(line);
		array = lsh_split_line(line, ",");
		if (ret == 0)
			return (ret);
		else if (!ft_strcmp(key, array[0]))
		{
			pos = lseek(fd, 0, SEEK_CUR);
			pos = lseek(fd, pos - delete_bytes - 1, SEEK_SET);
			printf("deleting [%d] bytes\n", delete_bytes);
			ft_bzero(line, delete_bytes);
			ret = write(fd, &line, delete_bytes);
			return (ret);
		}
		pos = lseek(fd, 0, SEEK_CUR);
	}
	return (ret);
}

/* Create Table Structure */

t_col	*new_header(char *name)
{
	t_col	*new;

	new = (t_col *)malloc(sizeof(t_col));
	new->data_count = 0;
	new->data = NULL;
	new->next = NULL;
	new->name = name;
	return (new);
}

t_col	*insrt_header(t_col *head, char *name)
{
	if (!head)
		head = new_header(name);
	else if (head)
		head->next = insrt_header(head->next, name);
	return (head);
}

t_tree	*new_datanode(int row, char *value)
{
	t_tree	*new;
	char	*cpy;

	cpy = ft_strnew(ft_strlen(value));
	ft_strcpy(cpy, value);
	new = (t_tree *)malloc(sizeof(t_tree));
	new->row = row;
	new->value = cpy;
	new->next = NULL;
	return (new);
}

t_tree	*insrt_datanode(t_col *col, t_tree *head, int row, char *val)
{
	if (!head)
	{
		col->data_count += 1;
		head = new_datanode(row, val);
	}
	else if (head)
		head->next = insrt_datanode(col, head->next, row, val);
	return (head);
}

t_table	*build_table(int fd)
{
	t_table	*table;
	char	*line;
	char	**array;
	int		ret;
	int		row;
	t_col	*temp;

	ret = 0;
	table = (t_table*)malloc(sizeof(t_table));

	/* setting meta data */
	table->header = NULL;
	get_next_line(fd, &line);
	array = lsh_split_line(line, ",");
	array++;
	while (*array)
	{
		table->header = insrt_header(table->header, *array);
		array++;
	}
	get_next_line(fd, &line);
	array = lsh_split_line(line, ",");
	table->col_count = ft_atoi(array[1]);
	row = 3;
	/* Setting Column data */
	while ((ret = get_next_line(fd, &line)) != -1)
	{
		if (ret == 0)
		{
			lseek(fd, 0, SEEK_SET);
			return (table);
		}
		temp = table->header;
		array = lsh_split_line(line, ",");
		if (ft_atoi(*array))
		{
			while (*array)
			{
				temp->data = insrt_datanode(temp, temp->data, row, *array);
				array++;
				temp = temp->next;
			}
		}
		row++;
	}
	lseek(fd, 0, SEEK_SET);
	return (table);
}

void	print_table(t_table table)
{
	printf("=============Printing Table=========\n");
	t_col	*temp;
	t_tree	*temp_data;

	temp = table.header;
	while (temp)
	{
		printf("%s->", temp->name);
		temp_data = temp->data;
		while (temp_data)
		{
			printf("(row#:%d, val:%s),", temp_data->row, (char *)temp_data->value);
			temp_data = temp_data->next;
		}
		printf("\n");
		temp = temp->next;
	}
	printf("=====End Table =======\n");
} 

/* Handle SQL Commands */

int	handle_insert(int argc, char **argv, person_rec *rec)
{
	int	fd;
	/* t_table	*table; */
	char	**array;

	if (argc ==  4 && !ft_strcmp(argv[1], "into"))
	{
		if((fd = open_record(argv[2])) == -1)
			return (0);
		array = lsh_split_line(argv[3], ",()");
		/* table = build_table(fd); */
		rec->key = array[1];
		rec->fname = ft_strnew(strlen(array[2]));
		strcpy(rec->fname, array[2]);
		rec->lname = ft_strnew(strlen(array[3]));
		strcpy(rec->lname, array[3]);
		rec->age = array[4];
		insert_record(fd, rec);
		close_record(fd);
		return (1);
	}
	(void)rec;
	return (0);
}

int	handle_delete(int argc, char **argv)
{
	int	fd;
	/* t_table	*table; */

	if (argc > 3 && !ft_strcmp(argv[1], "from"))
	{
		if((fd = open_record_delete(argv[2])) == -1)
			return (0);
		/* table = build_table(fd); */
		delete_record(fd, argv[3]);
		close_record(fd);
		return (1);
	}
	return (0);
}

/*------------------------------ select helper functions */
char *scan_for_file(char **argv)
{
	int i;

	i = 0;
	while (*argv[i])
	{
		if (!ft_strcmp(argv[i++], "from"))
		{
			if (argv[i])
				return (argv[i]);
		}
	}
	return (NULL);
}

char *set_scan_for_file(char **argv)
{
	int i;

	i = 0;
	while (*argv[i])
	{
		if (!ft_strcmp(argv[i++], "update"))
		{
			if (argv[i])
				return (argv[i]);
		}
	}
	return (NULL);
}

char	**parse_col_list(char **argv)
{
	int		i;
	int		j;
	char	**col_list;

	i = 1;
	col_list = NULL;
	while (ft_strcmp(argv[i++], "from"))
	{
	}
	col_list = (char **)malloc(sizeof(char *) * (i + 1));
	j = 0;
	i = 1;
	while (ft_strcmp(argv[i], "from"))
	{
		col_list[j++] = argv[i];
		i++;
	}
	col_list[j] = 0;
	return (col_list);
}

int	convert_to_index(int **colist_index, char **col_list, t_table *table)
{
	t_col	*temp_col;
	int		i;
	int		j;
	int		index;
	int		col_count;
	
	temp_col = table->header;
	i = 0;
	j = 0;
	while (col_list[i])
		i++;
	*colist_index = (int *)malloc(sizeof(int) * i);
	col_count = i;
	i = 0;
	while (col_list[i])
	{
		index = 0;
		temp_col = table->header;
		while (temp_col)
		{
			if (!strcmp(col_list[i], temp_col->name))
			{
				(*colist_index)[j] = index;
				j++;
				break;
			}
			index++;
			temp_col = temp_col->next;
		}
		i++;
	}
	return (col_count);
}

int	is_selected_column(int index, int *indexlist, int col_count)
{
	int i;
	int len;

	i = 0;
	len = col_count;
	while (i < len)
	{
		if (index == indexlist[i])
			return (1);
		i++;
	}
	return (0);
}

char	*get_constraint(char **argv)
{
	int i;
	char	*constraint;
	
	i = 0;
	constraint = NULL;
	while (argv[i] && ft_strcmp(argv[i], "WHERE"))
		i++;
	i++;
	if (argv[i])
		constraint = argv[i];
	return (constraint);
}

char	*get_set(char **argv)
{
	int i;
	char	*constraint;
	
	i = 0;
	constraint = NULL;
	while (argv[i] && ft_strcmp(argv[i], "set"))
		i++;
	i++;
	if (argv[i])
		constraint = argv[i];
	return (constraint);
}

void	print_record(int fd, int *row_list, char **col_list, t_table table)
{
	char 	*line;
	int		curr_row;
	int		i;
	char	**array;
	int		*colist_index;
	int		usr_col_count;
	/* person_rec	rec; */
	
	colist_index = NULL;
	curr_row = 0;
	i = 0;
	while (*row_list)
	{
		lseek(fd, 0, SEEK_SET);
		curr_row = 0;
		while (curr_row != *row_list && (get_next_line(fd, &line)) != -1)
			curr_row++;
		array = lsh_split_line(line, ",");
		if (!col_list)
		{
			while (*array)
			{
				printf("%s|", *array);
				array++;
			}
		}
		else
		{
			i = 0;
			usr_col_count = convert_to_index(&colist_index, col_list, &table);
			while (array[i])
			{
				if (is_selected_column(i, colist_index, usr_col_count))
					printf("%s|", array[i]);
				i++;
			}
		}
		printf("\n");
		free(line);
		row_list++;
	}
}

int	handle_select(int argc, char **argv)
{
	int			fd;
	/* int			i; */
	t_table		*table;
	char		*file;
	char		**col_list;
	char		*constraint;
	/* person_rec	rec; */
	int			*row_list;

	/* i = 1; */
	file = scan_for_file(argv);
	col_list = NULL;
	constraint = get_constraint(argv);
	if (argc > 3 && file)
	{
		if((fd = open_record(file)) == -1)
			return (0);
		table = build_table(fd);
		col_list = parse_col_list(argv);
		/* validate_column_list(col_list); */
		/* print_table(*table); */
		if (col_list)
		{
			if (!ft_strcmp(col_list[0], "*"))
			{
				row_list = select_record(col_list, table, constraint);
				print_record(fd, row_list, NULL, *table);
			}
			else
			{
				row_list = select_record(col_list, table, constraint);
				print_record(fd, row_list, col_list, *table);
			}
		}
		close_record(fd);
	}
	return (1);
}

void	set_print_record(int fd, int fd_a, int *row_list, char **col_list, t_table table, char *constraint)
{
	char 	*line;
	int		curr_row;
	int		i;
	char	**array;
	char 	**con_array;
	int		*colist_index;
	person_rec	*rec;
	
	(void)col_list;
	(void)table;
	colist_index = NULL;
	curr_row = 0;
	i = 0;
	while (*row_list)
	{
		lseek(fd, 0, SEEK_SET);
		lseek(fd_a, 0, SEEK_SET);
		curr_row = 0;
		while (curr_row != *row_list && (get_next_line(fd, &line)) != -1)
			curr_row++;
		array = lsh_split_line(line, ",");
		con_array = ft_strsplitstr(constraint, "=");
		rec = (person_rec*)malloc(sizeof(person_rec));
		rec->key = array[0];
		if (!strcmp(con_array[0], "fname"))
		{
			rec->fname = ft_strnew(strlen(con_array[1]));
			strcpy(rec->fname, con_array[1]);
		}
		else
		{
			rec->fname = ft_strnew(strlen(array[1]));
			strcpy(rec->fname, array[1]);
		}
		if (!strcmp(con_array[0], "lname"))
		{
			rec->lname = ft_strnew(strlen(con_array[1]));
			strcpy(rec->lname, con_array[1]);
		}
		else
		{
			rec->lname = ft_strnew(strlen(array[2]));
			strcpy(rec->lname, array[2]);
		}
		if (!strcmp(con_array[0], "age"))
			rec->age = con_array[1];
		else
			rec->age = array[3];
		delete_record(fd, array[0]);
		insert_record(fd_a, rec);
		printf("\n");
		free(line);
		row_list++;
	}
}

/* update roster.db set fname=potato WHERE fname=kim */
int handle_set(int argc, char **argv)
{
	int			fd;
	int 		fd_a;
	t_table		*table;
	char		*file;
	char		**col_list;
	char		*constraint;
	char 		*set_constraint;
	int			*row_list;

	file = set_scan_for_file(argv);
	col_list = (char **)malloc(sizeof(char *) * 2);
	col_list[0] = "*";
	col_list[1] = NULL;
	constraint = get_constraint(argv);
	set_constraint = argv[3];
	if (argc >= 6 && file)
	{
		if((fd = open_record_delete(file)) == -1)
			return (0);
		if((fd_a = open_record(file)) == -1)
			return (0);
		table = build_table(fd);
		row_list = select_record(col_list, table, constraint);
		set_print_record(fd, fd_a, row_list, NULL, *table, set_constraint);
		close_record(fd);
	}
	return (1);
}

/* MiniShell Section */
int 	sql(char *op, char **argv, char *line)
{
	int	argc;
	person_rec rec;

	argc = 0;
	argc = argc_len(argv);
	/* print_table(*table); */
	if (argc >= 1)
	{
		/* insert */
		if (!strcmp(op, "insert"))
		{
			if(!handle_insert(argc, argv, &rec))
			{
				printf("Error: Incomplete SQL:%s\n", line);
				return (1);
			}
		}
		/* delete */
		if (!strcmp(op, "delete"))
		{
			if(!handle_delete(argc, argv))
			{
				printf("Error: Incomplete SQL:%s\n", line);
				return (1);
			}
		}
		/* select */
		if (!strcmp(op, "select"))
		{
			if (!handle_select(argc, argv))
			{
				printf("Error: Incomplete SQL:%s\n", line);
				return (1);
			}
		}
		/* set */
		if (!strcmp(op, "update"))
		{
			if(!handle_set(argc, argv))
			{
				printf("Error: Incomplete SQL:%s\n", line);
				return (1);
			}
		}
	}
	return (1);
}

char	**lsh_split_line(char *line, const char *delim)
{
	int	bufsize = LSH_TOK_BUFSIZE;
	int	position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, delim);
	while (token != NULL) {
		tokens[position] = token; position++;

		if (position >= bufsize) {
			bufsize += LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, delim);
	}
	tokens[position] = NULL;
	return tokens;
}

int		sql_execute(char **args, char *line)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid < 0)
  {
	  // Error forking
  	perror("sql");
  }
  else if (pid == 0)
  {
    // Child process
    if (sql(args[0], args, line) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  }
  else
  {
    // Parent process
    do
	{
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

char	*lsh_read_line(void)
{
  char *line = NULL;
  /* size_t bufsize = 0; // have getline allocate a buffer for us */

  get_next_line(0, &line);
  /* getline(&line, &bufsize, stdin); */
  return line;
}

int main(void)
{
	char	*line;
	char	*user_input;
	char	**args;
	int		status;

	status = 1;
	line = NULL;
	do{
		ft_printf("SqLit\u27a6 ");
		line = lsh_read_line();
		user_input = strdup(line);
		args = lsh_split_line(line, " ");
		status = sql_execute(args, user_input);
		free(line);
		free(args);
	} while(status);
	return (0);
}
