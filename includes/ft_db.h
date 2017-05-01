/* ************************************************************************** */

/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_db.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcheung <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/04/25 08:05:28 by kcheung           #+#    #+#             */
/*   Updated: 2017/04/26 10:06:27 by kcheung          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DB_H
# define FT_DB_H

#include "libft.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " ,\t\r\n\a"
#define	LNAME 16
#define	FNAME 16

typedef struct	s_personrec
{
	char	*key;
	char	*fname;
	char	*lname;
	char	*age;
}				person_rec;

typedef struct	 s_tree
{
	int				row;
	void			*value;
	struct s_tree	*next;
}				t_tree;

typedef struct	s_col
{
	char			*name;
	char			*type;
	int				data_count;
	t_tree			*data;
	int				slected;
	struct s_col	*next;
}				t_col;

typedef struct	s_table
{
	t_col	*header;
	int		col_count;
	char	*primary_key;
	char	*foregin_key;
}				t_table;
#endif
