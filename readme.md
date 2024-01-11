# Klonowanie repozytorium

```
git clone git@github.com:mdretkie/psir-2023z.git
cd psir-2023z
```

# Kompilacja

```
./build-all.sh
```

# Uruchomienie

Jeśli to nie będzie działało, to w pliku `run.sh` należy zmienić adres IP z `10.0.2.15` na ten, który możemy odczytać `ip a`.

```
./run.sh server
./run.sh app1-master
./run.sh app1-worker
./run.sh app2-sensor
./run.sh app2-counter
```


