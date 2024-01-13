#set text(lang: "pl", font: "New Computer Modern")
#set par(justify: true)


#align(center + horizon)[
    #text(size: 36pt)[
	PSIR — Projekt
    ]
    #v(1cm)
    #text(size: 24pt)[
	Michał Drętkiewicz\
	Eryk Głąb\
        Patryk Kosiński
    ]
    #v(1cm)
    #text(size: 20pt)[
	13.01.2024
    ]
]

#pagebreak()

#set heading(numbering: "1.1")

#outline(
  indent: true,
  depth: 3
)

#pagebreak()
#set page(numbering: "1")


= Specyfikacja protokołu aplikacyjnego

Implementacja protokołu zawarta jest w plikach `protocol.h` i `protocol.h`.

Wiadomości protokołu mają następującą strukturę:
```c
typedef struct Message {
    uint32_t id;
    MessageType type;
    MessageData data;
} Message;
```

Znaczenie pól:

- *`id`* -- jest to unikalny w zakresie każdego połączenie identyfikator wiadomości, wykorzystywany głównie w mechanizmie potwierdzania odbioru (ACK). Kolejne wartości są generowane funkcją `message_next_id()`.

- *`type`* -- rodzaj wiadomości. Definicja typu `MessageType`:

    ```c
    typedef enum MessageType {
        message_ack,
        message_tuple_space_insert_request,
        message_tuple_space_get_request,
        message_tuple_space_get_reply,
    } MessageType;
    ```
    
    Znaczenie wartości:

        - *`message_ack`* -- potwierdzenie odebrania wcześniejszej wiadomości, przesyłane w obu kierunkach,

        - *`message_tuple_space_insert_request`* -- żądanie wstawienia krotki do przestrzeni krotek, wysyłane od klienta do serwera,

        - *`message_tuple_space_get_request`* -- żądanie odczytania krotki z przestrzeni krotek, wysyłane od klienta do serwera,

        - *`message_tuple_space_get_reply`* -- odpowiedź na wiadomość typu `message_tuple_space_get_request`, przesyłana od serwera do klienta.

- *`data`* -- reszta danych wiadomości, zależna od jej typu. Definicja typu `MessageData`:
    ```c
    typedef union MessageData {
        MessageAck ack;
        MessageTupleSpaceInsertRequest tuple_space_insert_request;
        MessageTupleSpaceGetRequest tuple_space_get_request;
        MessageTupleSpaceGetReply tuple_space_get_reply;
    } MessageData;
    ```

    Poszczególny pola unii odpowiadają kolejno wartościom `MessageType` opisanym w poprzednim punkcie. 

    Definicje typów pól:
      - ```c
        typedef struct MessageAck {
            uint32_t message_id;
        } MessageAck;
        ```

        Pole *`message_id`* to `id` wiadomości, której dotyczy to potwierdzenie.

      - ```c
        typedef struct MessageTupleSpaceInsertRequest {
            Tuple tuple;
        } MessageTupleSpaceInsertRequest;
        ```

        Znaczenie pola takie samo, jak w argumencie funkcji `tuple_space_insert()`, opisanej w następnym rozdziale.

      - ```c
        typedef struct MessageTupleSpaceGetRequest {
            Tuple tuple_template;
            TupleSpaceOperationBlockingMode blocking_mode; 
            TupleSpaceOperationRemovePolicy remove_policy;
        } MessageTupleSpaceGetRequest;
        ```

        Znaczenie pól takie samo, jak w argumentach funkcji `tuple_space_get()`, opisanej w następnym rozdziale.


        ```c
        typedef struct MessageTupleSpaceGetReply {
            TupleSpaceOperationResult result;
        } MessageTupleSpaceGetReply;
        ```

        Znaczenie pola takie samo, jak w wartości zwracanej z funkcji `tuple_space_get()`, opisanej w następnym rozdziale.


Podczas transmisji, każda wiadomość poprzedzona jest swoją długością w bajtach, rzutowaną na typ `uint32_t`.


= Tuple Space

Tuple Space definiuje następujące API, zaimplementowane w plikach `tuple_space.h` i `tuple_space.c`:

- Struktura utrzymująca stan przestrzeni krotek:
  ```c
typedef struct TupleSpace {
    size_t tuple_count;
    Tuple* tuples;
} TupleSpace;
  ```

- Konstruktor pustej przestrzeni krotek:
  ```c
TupleSpace tuple_space_new();
  ```

- Destruktor przestrzeni krotek, zwalniający zasoby wszystkich przechowywanych.
  ```c
void tuple_space_free(TupleSpace tuple_space);
  ```

- Metoda wstawiająca krotkę `tuple` do przestrzeni krotek. Nieblokująca. Nie ma wymagania, aby wstawiane krotki były różne.
  ```c
void tuple_space_insert(TupleSpace* tuple_space, Tuple tuple);
  ```

- Metoda pozyskująca z przestrzeni krotek krotkę pasującą do wzorca `tuple_template`. Jeśli pasuje więcej niż jedna, zwrócona będzie arbitralnie wybrana z nich.
  ```c
TupleSpaceOperationResult tuple_space_get(
    TupleSpace* tuple_space, 
    Tuple const* tuple_template, 
    TupleSpaceOperationBlockingMode blocking_mode, 
    TupleSpaceOperationRemovePolicy remove_policy
);
  ```

  Parametr `blocking_mode` określa, czy funkcja zablokuje wywołujący ją wątek, gdy w przestrzeni nie będzie krotki pasującej do wzorca. Zdefiniowane są następujące wartości, które oznaczają odpowiednio operację blokująca i nieblokującą:
  ```c
typedef enum TupleSpaceOperationBlockingMode {
    tuple_space_blocking, 
    tuple_space_nonblocking,
} TupleSpaceOperationBlockingMode;
  ```
  
  Parametr `remove_policy` określa czy w przypadku udanej operacji wybrana krotka ma zostać usunięta z przestrzeni krotek. Zdefiniowane są następujące wartości, które kolejno odpowiadają usunięciu krotki z przestrzeni i pozostawieniu jej:
  ```c
typedef enum TupleSpaceOperationRemovePolicy {
    tuple_space_remove, 
    tuple_space_keep,
} TupleSpaceOperationRemovePolicy;
  ```

  Metoda `tuple_space_get()` zwraca obiekt typu `TupleSpaceOperationResult`:
  ```c
typedef struct TupleSpaceOperationResult {
    TupleSpaceOperationStatus status;
    Tuple tuple;
} TupleSpaceOperationResult;
  ```

  Pole `status` oznacza odpowiednio powodzenie lub niepowodzenie operacji. W przypadku operacji blokującej, będzie to zawsze `tuple_space_success`, ponieważ metoda nie zakończy się, aż w przestrzeni będzie krotka pasująca do wzorca. W przypadku operacji nieblokującej, w sytuacji, gdy w przestrzeni nie ma odpowiedniej krotki, pole to ustawiane jest na `tuple_space_failure`.
  ```c
typedef enum TupleSpaceOperationStatus {
    tuple_space_success, 
    tuple_space_failure,
} TupleSpaceOperationStatus;
  ```

  Pole `tuple` to wynikowa krotka. W przypadku niepowodzenia operacji nieblokującej będzie to zawsze pusta (0-elementowa) krotka.

  - Metoda konwertująca `TupleSpace` do postaci tekstowej:
  ```c
char const* tuple_space_to_string(TupleSpace const* tuple_space);
  ```
  Wynikowy string jest poprawny tylko do momentu kolejnego wywołania tej metody z tego samego wątku. Aby zachować go na dłużej należy wykonać kopię.


Struktura `TupleSpace` i jej wszystkie metody są thread-safe (można je wykonywać współbieżnie), ponieważ według pierwotnego pomysłu serwer miał być zaimplementowany wielowątkowo, co jednak nie okazało się konieczne.


= Serwer




= Aplikacja 1.

= Aplikacja 2.

= Environment file dla aplikacji 2.
