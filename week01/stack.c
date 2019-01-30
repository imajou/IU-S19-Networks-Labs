#include "stack.h"

struct Node *STACK_ROOT = NULL;
unsigned int size = 0;

struct Node
{
    int value;
    struct Node *next;
};

/**
 * Creates new node with data {@param value} and returns a pointer to it
 * @param value Value to put in a new node
 * @return Pointer to a new node
 */
struct Node *createNode(int value)
{
    struct Node *newNode =
        (struct Node *)malloc(sizeof(struct Node));
    newNode->value = value;
    newNode->next = NULL;

    return newNode;
}

/**
 * Pops a value from the top of the stack
 * If the stack is empty, prints an error message and returns minus infinity
 * @return Top value or -INF if the stack is empty
 */
int pop()
{
    if (!STACK_ROOT)
    {
        printf("Stack is empty\n");
        return -INFINITY;
    }

    struct Node *temp = STACK_ROOT;
    STACK_ROOT = STACK_ROOT->next;
    int popped = temp->value;
    free(temp);

    --size;
    return popped;
}

/**
 * Peeks a value from the top of the stack
 * If stack is empty, prints an error message and returns minus infinity
 * @return Top value or -INF if stack is empty
 */
int peek()
{
    if (!STACK_ROOT)
    {
        printf("Stack is empty\n");
        return -INFINITY;
    }
    return STACK_ROOT->value;
}

/**
 * Pushes {@param value} to the top of the stack and {@return status} of pushing
 * {@return 0} if everything is ok, {@return 1} if stack is full, {@return 2} if some error occurred
 * @param value Value to push
 * @return Status code
 */
void push(int value)
{
    struct Node *stackNode = createNode(value);
    stackNode->next = STACK_ROOT;
    STACK_ROOT = stackNode;

    ++size;
}

/**
 * Check if stack is empty
 * @return stack is empty
 */
int empty()
{
    return (size == 0) ? TRUE : FALSE;
}

/**
 * Print stack
 * @return stack values array
 */
int *display()
{
    int output[size];
    struct Node *current_node = STACK_ROOT;
    for (int i = 0; i < size; i++)
    {
        output[i] = current_node->value;
        current_node = current_node->next;
    }
    return output;
}

/**
 * Reinitialize stack
 */
void create()
{
    STACK_ROOT = NULL;
    size = 0;
}

/**********************************************************************************************************************/

static int pipe_descriptor[2];
static pid_t server_pid;

char *pipe_read()
{
    char l_buffer[BUFFER_SIZE];
    close(pipe_descriptor[1]);
    read(pipe_descriptor[0], l_buffer, BUFFER_SIZE);
    close(pipe_descriptor[0]);
    return l_buffer;
}

void pipe_write(char *input)
{
    close(pipe_descriptor[0]);
    write(pipe_descriptor[1], input, BUFFER_SIZE);
    close(pipe_descriptor[1]);
}

/**********************************************************************************************************************/

void cont_server()
{
    kill(server_pid, SIGCONT);
}

void stop_server()
{
    kill(server_pid, SIGSTOP);
}

void wait_server_output()
{
    waitpid(server_pid, NULL, WUNTRACED);
    printf("%s\n", pipe_read());
}

char *execute_command(char *input)
{

    int data = 0;
    char *output = malloc(BUFFER_SIZE * sizeof(char));

    int isWriting = FALSE;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (input[i] == ')')
            break;
        if (isWriting)
            data = data * 10 + ((int)input[i]);
        if (input[i] == '(')
            isWriting = TRUE;
    }

    if (strstr(input, COMMAND_PEEK) != NULL)
    {
        sprintf(output, "Peeked: %d\n", peek());
    }

    if (strstr(input, COMMAND_PUSH) != NULL)
    {
        sprintf(output, "Pushed: %d\n", data);
        push(data);
    }

    if (strstr(input, COMMAND_POP) != NULL)
    {
        sprintf(output, "Popped: %d\n", pop());
    }

    if (strstr(input, COMMAND_EMPTY) != NULL)
    {
        sprintf(output, "Stack is empty: %d\n", empty());
    }

    if (strstr(input, COMMAND_DISPLAY) != NULL)
    {
        sprintf(output, "Stack: %s\n", display());
    }

    if (strstr(input, COMMAND_CREATE) != NULL)
    {
        sprintf(output, "Initialized stack\n");
        create();
    }

    if (strstr(input, COMMAND_STACK_SIZE) != NULL)
    {
        sprintf(output, "Stack size: %d\n", size);
    }

    return output;
}

void process_command()
{
    char *input = pipe_read();
    char *output = execute_command(input);
    pipe_write(output);
}

void sigusrHandler(int signum)
{
    process_command();
    kill(getppid(), SIGUSR1);
}

void start_server()
{
    signal(SIGUSR1, sigusrHandler);

    while (1)
        ;
}

/**********************************************************************************************************************/

void start_client()
{
    stop_server();

    printf("Welcome to GleSTACK v0.1!\nEnter \"exit\" to finish the process.\n");

    while (1)
    {
        printf("glestack-0.1$ ");
        char input[BUFFER_SIZE] = "";
        fgets(input, BUFFER_SIZE, stdin);
        if (!strcmp(input, "exit\n"))
        {
            exit(0);
        }
        else
        {
            printf("Writing command to pipe...\n");
            pipe_write(input);

            printf("Resuming server...\n");
            cont_server();

            kill(server_pid, SIGUSR1);

            printf("Waiting for output...\n");
            wait_server_output();

            printf("Stopping server...\n");
            stop_server();
        }
    }
}

/**********************************************************************************************************************/

int main()
{
    pipe(pipe_descriptor);

    server_pid = fork();

    if (server_pid == 0)
    {
        start_server();
    }
    else
    {
        start_client();
    }

    return 0;
}
