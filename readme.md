## Пример по курсу Архитектура Программных систем


```plantuml
@startuml

component browser
component hl_mai_06_cache
database my_sql
database redis

browser -d-> hl_mai_06_cache : http://localhost/author?id=some_id
hl_mai_06_cache -d-> redis : get key [tcp:6379]
hl_mai_06_cache -d-> my_sql : select * from author where id=some_id [tcp:3306]
hl_mai_06_cache -d-> redis : put key [tcp:6379]


@enduml
```


Тестовый запрос для добавления в MySQL и кэш:

http://localhost/author?add&first_name=*Vasia*&last_name=Pupkin&title=Mr&email=vasia@pupkin.com

Тестовый запрос для поиска в кэше:

http://*localhost*/author?id=*some_id -* поиск записей (шаблон сквозное-чтение)

http://*localhost*/author?id=*some_id&no_cache -* поиск записей буз кеша

### Бибилиотека для работы с redis
https://github.com/tdv/redis-cpp