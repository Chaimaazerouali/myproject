#include "shell.h"

/**
 * is_my_chain - test if the current character in the buffer is a chain delimiter
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of the current position in buf
 *
 * Return: 1 if chain delimiter, 0 otherwise
 */
int is_my_chain(my_info_t *info, char *buf, size_t *p)
{
    size_t j = *p;

    if (buf[j] == '|' && buf[j + 1] == '|')
    {
        buf[j] = 0;
        j++;
        info->cmd_buf_type = MY_CMD_OR;
    }
    else if (buf[j] == '&' && buf[j + 1] == '&')
    {
        buf[j] = 0;
        j++;
        info->cmd_buf_type = MY_CMD_AND;
    }
    else if (buf[j] == ';') /* found end of this command */
    {
        buf[j] = 0; /* replace semicolon with null */
        info->cmd_buf_type = MY_CMD_CHAIN;
    }
    else
        return (0);
    *p = j;
    return (1);
}

/**
 * check_my_chain - checks if we should continue chaining based on the last status
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of the current position in buf
 * @i: starting position in buf
 * @len: length of buf
 *
 * Return: Void
 */
void check_my_chain(my_info_t *info, char *buf, size_t *p, size_t i, size_t len)
{
    size_t j = *p;

    if (info->cmd_buf_type == MY_CMD_AND)
    {
        if (info->status)
        {
            buf[i] = 0;
            j = len;
        }
    }
    if (info->cmd_buf_type == MY_CMD_OR)
    {
        if (!info->status)
        {
            buf[i] = 0;
            j = len;
        }
    }

    *p = j;
}

/**
 * replace_my_alias - replaces aliases in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_my_alias(my_info_t *info)
{
    int i;
    my_list_t *node;
    char *p;

    for (i = 0; i < 10; i++)
    {
        node = my_node_starts_with(info->alias, info->argv[0], '=');
        if (!node)
            return (0);
        free(info->argv[0]);
        p = my_strchr(node->str, '=');
        if (!p)
            return (0);
        p = my_strdup(p + 1);
        if (!p)
            return (0);
        info->argv[0] = p;
    }
    return (1);
}

/**
 * replace_my_vars - replaces variables in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_my_vars(my_info_t *info)
{
    int i = 0;
    my_list_t *node;

    for (i = 0; info->argv[i]; i++)
    {
        if (info->argv[i][0] != '$' || !info->argv[i][1])
            continue;

        if (!my_strcmp(info->argv[i], "$?"))
        {
            replace_my_string(&(info->argv[i]),
                    my_strdup(convert_my_number(info->status, 10, 0)));
            continue;
        }
        if (!my_strcmp(info->argv[i], "$$"))
        {
            replace_my_string(&(info->argv[i]),
                    my_strdup(convert_my_number(getpid(), 10, 0)));
            continue;
        }
        node = my_node_starts_with(info->env, &info->argv[i][1], '=');
        if (node)
        {
            replace_my_string(&(info->argv[i]),
                    my_strdup(my_strchr(node->str, '=') + 1));
            continue;
        }
        replace_my_string(&info->argv[i], my_strdup(""));
    }
    return (0);
}

/**
 * replace_my_string - replaces a string
 * @old: address of the old string
 * @new: new string
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_my_string(char **old, char *new)
{
    free(*old);
    *old = new;
    return (1);
}

