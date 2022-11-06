// Shell для Arduino
// работает через COM-порт

#define RL_BUFSIZE 1024 // Размер буфера ввода
#define TOK_BUFSIZE 64  // Размер буфера с указателями на токены
#define TOK_DELIM " \t\r\n\a" // Разделители для токенизации

int cmd_led(char **args);
int cmd_help(char **args);
int cmd_exit(char **args);

char *line; // Указатель на строку ввода
char **args; // Указатель на указатель на аргументы

// Массив имен встроенных команд
char *cmd_str[] = {
	"led",           // Включение светодиода
	"help",			     // Справка
	"exit"			     // Выход
};

// Массив указателей на функции встроенных команд
int (*cmd_func[]) (char **) = {
	cmd_led,
	cmd_help,
	cmd_exit
};


// Возвращает указатель на строку ввода
char *read_line(void){
  int bufsize = RL_BUFSIZE; // Размер буфера ввода
  char *buffer = malloc(sizeof(char) * bufsize); // Выделяем память под буфер ввода

  // Проверяет, выделилась ли память под буфер ввода
  if(!buffer){
    Serial.println("Error allocation");
  }

  // Читаем по символу в буфер до конца строки или файла
  for(int position = 0; position < RL_BUFSIZE; position++)
  {
    while(Serial.available()==0);  //Ожидаем ввода данных
    buffer[position]=Serial.read();
    Serial.flush();
    if(buffer[position]=='\n'){
      buffer[position]=0; 
      return buffer;
    }
  }  
}


// Извлекает аргументы
char **split_line(char *line){
  int bufsize = TOK_BUFSIZE;
  int position = 0;
  char **tokens = malloc(sizeof(char*) * bufsize);
  char *token;

  // Проверяет, выделилась ли память
  if(!tokens){
    Serial.println("Error allocation");
  }

  // Получаем первый токен из буфера ввода
	token = strtok(line, TOK_DELIM);
	// Заполняем в цикле указатель на указатели на токены
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		// Получаем последующие токены (поэтому NULL)
		token = strtok(NULL, TOK_DELIM);
	}
	// Последний токен NULL
	tokens[position] = NULL;
	return tokens;
}


// Запуск процессов
int launch(char **args) {
	// Здесь должен быть запуск процессов
  Serial.println("Launched");
	return 1;
}


int num_builtins() {
		return sizeof(cmd_str) / sizeof(char *);
}


// Выполнение встроенных команд
int execute(char **args){
  int i;
	
	// Проверяем, не путая ли строка введена
	if (args[0] == NULL) {
		return 1;
	}
	
	for (i = 0; i < num_builtins(); i++) {
		if (strcmp(args[0], cmd_str[i]) == 0) {
			return (*cmd_func[i])(args);
		}
	}
	
  return launch(args);
}


// Команда включения светодиода
int cmd_led(char **args) {
	// Проверяем, есть ли второй аргумент
	if (args[1] == NULL) {
		// Сообщение об ошибке
    Serial.println("Expected argument (on/off) to \"led\"");
	}
	else {
    String str = args[1];
    //Serial.println(str);    
    if (str == "on") {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Led ON"); 
	  }
    if (str == "off") {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Led OFF"); 
	  }
	}
	return 1;
}


// Команда help
int cmd_help(char **args) {
	int i;
  Serial.println("Shell v.1.0 (c) 2022 Gor.Com");
	Serial.println("Type program names and arguments, and hit enter.");
	Serial.println("The following are built in:");
	
	// Выводим массив имен встроенных команд
	for (i = 0; i < num_builtins(); i++) {
		Serial.println(cmd_str[i]);
	}
	return 1;
}


// Команда выход
int cmd_exit(char **args) {
  Serial.println("Executed EXIT");
	return 0;
}


// Инициализация
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // pin 13 - светодиод

  Serial.begin(9600); // Открываем COM-порт, скорость 9600 bps
  while (!Serial); // Ждем пока Serial будет готов
  Serial.println("Shell v.1.0 (c) 2022 Gor.Com");  
}


void loop() {
  Serial.print(">");        // Печатает приглашение ввода команды   
  line = read_line();       // Получаем указатель на строку ввода

  Serial.println(line);
  
  args = split_line(line);  // Извлекает аргументы

  //Serial.println(args[0]);
  //Serial.println(args[1]);
  //Serial.println(args[2]);

  execute(args);            // Исполняем команду  

  // Освобождает память
  free(line);
  free(args);
}
