## Steps to compile
```sh
  mkdir build/
  cd build/
  cmake ..
  make
```

## How to run it
```sh
  ./cli cities.txt
```
where cities.txt is just a file with set of lines made with below pattern : [continent] [country] [city]

So similarily using Prompt::insertMapElement() you can easiy create your own menu structure, where each command that contains a set of words, has corresponding function callback attached to it.
That's it ;-)


