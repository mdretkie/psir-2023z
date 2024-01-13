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


= Serwer


= Aplikacja 1.

= Aplikacja 2.

= Environment file dla aplikacji 2.
