#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 2
#define MAX_COMMAND_LENGTH 50
#define MAX_DESCRIPTION_LENGTH 200

/* Status of a task */
typedef enum
{
    PENDING,
    DONE
} TaskStatus;

/* Supported commands */
typedef enum
{
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

/* Task structure */
typedef struct
{
    int id;
    char description[MAX_DESCRIPTION_LENGTH];
    TaskStatus status;
} Task;

/* Remove the trailing newline from an input line so the string can be stored and compared cleanly. */
void remove_newline(char *str)
{
    str[strcspn(str, "\n")] = '\0';
}

/* If the user entered more characters than the buffer can hold, discard the remaining input.
   This prevents the next fgets() call from reading leftover characters from the same line. */
void flush_input_if_needed(char *buffer)
{
    if (strchr(buffer, '\n') == NULL)
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("Warning: Input exceeded limit and was truncated.\n");
    }
}

/* Convert a string to uppercase so commands like "add" and "ADD" are treated the same. */
void to_uppercase(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

/* Search through the task array and return the index matching the given task ID.
   Returns -1 when the ID is not found. */
int find_task_index(Task *tasks, int count, int id)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (tasks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

/* Map a user command string to the corresponding command enum value. */
CommandType get_command_type(char *command)
{
    to_uppercase(command);

    if (strcmp(command, "ADD") == 0)
        return CMD_ADD;

    if (strcmp(command, "DELETE") == 0)
        return CMD_DELETE;

    if (strcmp(command, "VIEW") == 0)
        return CMD_VIEW;

    if (strcmp(command, "UPDATE") == 0)
        return CMD_UPDATE;

    if (strcmp(command, "COMPLETE") == 0)
        return CMD_COMPLETE;

    if (strcmp(command, "FIND") == 0)
        return CMD_FIND;

    if (strcmp(command, "HELP") == 0)
        return CMD_HELP;

    if (strcmp(command, "EXIT") == 0)
        return CMD_EXIT;

    return CMD_UNKNOWN;
}

/* Display the command list so the user knows which actions are supported. */
void show_help()
{
    printf("\n========== HELP ==========\n");
    printf("ADD       - Add a new task\n");
    printf("DELETE    - Delete a task\n");
    printf("VIEW      - View all tasks\n");
    printf("UPDATE    - Update task description\n");
    printf("COMPLETE  - Mark task as completed\n");
    printf("FIND      - Search task by ID\n");
    printf("HELP      - Show all commands\n");
    printf("EXIT      - Exit program\n");
    printf("==========================\n");
}

/* Add a new task after validating the ID, description, and duplicate check.
   If the array is full, resize it dynamically so more tasks can be stored. */
void add_task(Task **tasks, int *count, int *capacity)
{
    char buffer[100];
    char description[MAX_DESCRIPTION_LENGTH];
    int id;

    printf("Enter Task ID: ");

    if (!fgets(buffer, sizeof(buffer), stdin))
        return;

    flush_input_if_needed(buffer);

    if (sscanf(buffer, "%d", &id) != 1 || id <= 0)
    {
        printf("Invalid task ID.\n");
        return;
    }

    if (find_task_index(*tasks, *count, id) != -1)
    {
        printf("Error: Duplicate task ID.\n");
        return;
    }

    printf("Enter Description: ");

    if (!fgets(description, sizeof(description), stdin))
        return;

    flush_input_if_needed(description);
    remove_newline(description);

    if (strlen(description) == 0)
    {
        printf("Description cannot be empty.\n");
        return;
    }

    if (*count >= *capacity)
    {
        int new_capacity = (*capacity) * 2;

        Task *temp =
            realloc(*tasks, new_capacity * sizeof(Task));

        if (temp == NULL)
        {
            printf("Memory reallocation failed.\n");
            return;
        }

        *tasks = temp;
        *capacity = new_capacity;

        printf("Storage expanded to %d tasks.\n",
               *capacity);
    }

    (*tasks)[*count].id = id;

    strcpy((*tasks)[*count].description,
           description);

    (*tasks)[*count].status = PENDING;

    (*count)++;

    printf("Task %d added successfully.\n", id);
}

/* Remove a task by ID and shift later entries forward to keep the array compact. */
void delete_task(Task *tasks, int *count)
{
    char buffer[100];
    int id;
    int i;

    if (*count == 0)
    {
        printf("Task list is empty.\n");
        return;
    }

    printf("Enter Task ID to delete: ");

    if (!fgets(buffer, sizeof(buffer), stdin))
        return;

    if (sscanf(buffer, "%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    int index =
        find_task_index(tasks, *count, id);

    if (index == -1)
    {
        printf("Task not found.\n");
        return;
    }

    for (i = index; i < *count - 1; i++)
    {
        tasks[i] = tasks[i + 1];
    }

    (*count)--;

    printf("Task %d deleted successfully.\n", id);
}

/* Update the description of an existing task after confirming that it exists. */
void update_task(Task *tasks, int count)
{
    char buffer[100];
    char description[MAX_DESCRIPTION_LENGTH];
    int id;

    if (count == 0)
    {
        printf("Task list is empty.\n");
        return;
    }

    printf("Enter Task ID: ");

    if (!fgets(buffer, sizeof(buffer), stdin))
        return;

    if (sscanf(buffer, "%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    int index =
        find_task_index(tasks, count, id);

    if (index == -1)
    {
        printf("Task not found.\n");
        return;
    }

    printf("Enter New Description: ");

    if (!fgets(description,
               sizeof(description),
               stdin))
        return;

    flush_input_if_needed(description);
    remove_newline(description);

    if (strlen(description) == 0)
    {
        printf("Description cannot be empty.\n");
        return;
    }

    strcpy(tasks[index].description,
           description);

    printf("Task %d updated successfully.\n", id);
}

/* Mark a task as completed by changing its status field from PENDING to DONE. */
void complete_task(Task *tasks, int count)
{
    char buffer[100];
    int id;

    if (count == 0)
    {
        printf("Task list is empty.\n");
        return;
    }

    printf("Enter Task ID: ");

    if (!fgets(buffer, sizeof(buffer), stdin))
        return;

    if (sscanf(buffer, "%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    int index =
        find_task_index(tasks, count, id);

    if (index == -1)
    {
        printf("Task not found.\n");
        return;
    }

    tasks[index].status = DONE;

    printf("Task %d marked as DONE.\n", id);
}

/* Display the details of a single task using its unique task ID. */
void find_task(Task *tasks, int count)
{
    char buffer[100];
    int id;

    if (count == 0)
    {
        printf("Task list is empty.\n");
        return;
    }

    printf("Enter Task ID: ");

    if (!fgets(buffer, sizeof(buffer), stdin))
        return;

    if (sscanf(buffer, "%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    int index =
        find_task_index(tasks, count, id);

    if (index == -1)
    {
        printf("Task not found.\n");
        return;
    }

    printf("\nTask Found\n");
    printf("-------------------------\n");
    printf("ID          : %d\n",
           tasks[index].id);
    printf("Description : %s\n",
           tasks[index].description);
    printf("Status      : %s\n",
           tasks[index].status == DONE
               ? "DONE"
               : "PENDING");
}

/* Print all tasks in a simple table layout to help the user review the list quickly. */
void view_tasks(Task *tasks, int count)
{
    int i;

    if (count == 0)
    {
        printf("Task list is empty.\n");
        return;
    }

    printf("\n====================================================\n");
    printf("%-10s %-30s %-10s\n",
           "ID",
           "DESCRIPTION",
           "STATUS");
    printf("====================================================\n");

    for (i = 0; i < count; i++)
    {
        printf("%-10d %-30s %-10s\n",
               tasks[i].id,
               tasks[i].description,
               tasks[i].status == DONE
                    ? "DONE"
                    : "PENDING");
    }

    printf("====================================================\n");
}

/* Main program loop: read commands, parse them, and call the matching task-management action. */
int main()
{
    Task *tasks;
    int count = 0;
    int capacity = INITIAL_CAPACITY;

    char command[MAX_COMMAND_LENGTH];

    tasks = malloc(capacity * sizeof(Task));

    if (tasks == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("====================================\n");
    printf(" INTERACTIVE TASK LIST MANAGER\n");
    printf(" Type HELP for command list.\n");
    printf("====================================\n");

    /* Keep reading commands until the user exits the program. */
    while (1)
    {
        printf("\nCommand > ");

        if (!fgets(command,
                   sizeof(command),
                   stdin))
        {
            printf("\nEOF detected. Exiting...\n");
            break;
        }

        flush_input_if_needed(command);
        remove_newline(command);

        CommandType cmd =
            get_command_type(command);

        switch (cmd)
        {
        case CMD_ADD:
            add_task(&tasks,
                     &count,
                     &capacity);
            break;

        case CMD_DELETE:
            delete_task(tasks,
                        &count);
            break;

        case CMD_VIEW:
            view_tasks(tasks,
                       count);
            break;

        case CMD_UPDATE:
            update_task(tasks,
                        count);
            break;

        case CMD_COMPLETE:
            complete_task(tasks,
                          count);
            break;

        case CMD_FIND:
            find_task(tasks,
                      count);
            break;

        case CMD_HELP:
            show_help();
            break;

        case CMD_EXIT:
            printf("Exiting program.\n");
            free(tasks);
            return 0;

        default:
            printf("Unknown command. Type HELP.\n");
        }
    }

    free(tasks);

    return 0;
}
