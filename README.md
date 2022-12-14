
<h1 align="center">
🌐WebServer
</h1>

## 💡 О проекте:

> _Этот проект посвящен написанию собственного HTTP-сервера. Вы сможете протестировать
>  его с помощью реального браузера.
>  HTTP - один из самых используемых протоколов в Интернете. Знание его тонкостей будет полезно, даже если вы не будете работать над вебсайтом._

		Протокол передачи гипертекста (HTTP) - это прикладной протокол для распределенных,
	совместных, гипермедийных информационных систем. 
		HTTP - это основа передачи данных во Всемирной паутине, где гипертекстовые документы
	содержат гиперссылки на другие ресурсы, к которым пользователь может легко получить
	доступ. Например, щелчком мыши или касанием экрана в веб-браузере.
		В этом проете необходимо реализовать HTTP сервер при помощи socket на С++98


Базовая часть:
-  Программа должна принимать конфигурационный файл в качествеаргумента или использовать путь по умолчанию.
- Сервер никогда не должен блокироваться, и при необходимости клиент может быть отшит должным образом.
- Он должен быть неблокирующим и использовать только 1 poll() (или эквивалент) для всех операций ввода-вывода между клиентом и сервером (включая прослушивание).
-  poll() (или эквивалент) должен проверять чтение и запись одновременно.
-  Вы никогда не должны выполнять операции чтения или записи, не пройдя через poll() (или эквивалент).
- Проверять значение errno после операции чтения или записи строго запрещено.
-  Вы можете использовать все макросы и определения, такие как FD_SET, FD_CLR, FD_ISSET, FD_ZERO (понимание того, что и как они делают, очень полезно).
- Запрос к вашему серверу никогда не должен зависать надолго.
- Ваш сервер должен быть совместим с выбранным вами веб-браузером.
- Мы рассмотрим, что NGINX совместим с HTTP 1.1 и может быть использован
для сравнения заголовков и поведения ответов.
- Ваши коды состояния HTTP-ответов должны быть точными.
- На вашем сервере должны быть страницы ошибок по умолчанию, если
таковые не предусмотрены.
- Вы не можете использовать fork для чего-то другого, кроме CGI (например, PHP,
или Python, и так далее).
- Вы должны уметь обслуживать полностью статичный сайт.
- Вам нужны как минимум методы GET, POST и DELETE.
- Стресс-тесты вашего сервера. Он должен оставаться доступным любой ценой.
- Ваш сервер должен иметь возможность прослушивать несколько портов.

Бонусная часть:
 *  Поддержка cookies и управления сессиями (подготовьте краткие примеры).
 * Обработка нескольких CGI.
	
Более подробно о проекте и нюансах можно посмотреть в  [**subject**](https://github.com/AYglazk0v/webserver/blob/main/additionally/en.subject.pdf).

## 🛠 Тестирование и использование:

	# Клонируйте проект и получите доступ к папке
	git clone git@github.com:AYglazk0v/webserver.git && cd webserver/

	# Для сборки необходимо выполнить команду:
	make
	
	# Для сборки и отладки программы необходимо выполнить команду:
	make DEBUG=0
	
	# Для тестирования:
	./webserv {config}
	
	# Для того, чтобы пересобрать проект:
	make re
	
	# Для того, чтобы удалить объектные файлы:
	make clean
	
	# Для того, чтобы полностью очистить систему после тестирования:
	make fclean
		
## 🎬 Демонстрация:
![webserv](https://github.com/AYglazk0v/webserver/blob/main/additionally/720p.gif)
