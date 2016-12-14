#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

class VirusAlreadCreated {};
class VirusNotFound {};
class TriedToRemoveStemVirus {};

template <class Virus> class VirusGenealogy {
private:
  //Klasa Virus reprezentuje informacje o wirusie. Jej implementacja
  //zostanie dostarczona w stosownym czasie.
public:
  //Klasa VirusGenealogy powinna udostępniać następujący interfejs.

  // Tworzy nową genealogię.
  // Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
  VirusGenealogy(Virus::id_type const &stem_id);

  // Zwraca identyfikator wirusa macierzystego.
  Virus::id_type get_stem_id() const;

  // Zwraca listę identyfikatorów bezpośrednich następników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<Virus::id_type> get_children(Virus::id_type const &id) const;

  // Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<Virus::id_type> get_parents(Virus::id_type const &id) const;

  // Sprawdza, czy wirus o podanym identyfikatorze istnieje.
  bool exists(Virus::id_type const &id) const;

  // Zwraca referencję do obiektu reprezentującego wirus o podanym
  // identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  Virus& operator[](Virus::id_type const &id) const;

  // Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
  // powstały z wirusów o podanym identyfikatorze parent_id lub
  // podanych identyfikatorach parent_ids.
  // Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
  // id już istnieje.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
  // poprzedników nie istnieje.
  void create(Virus::id_type const &id, Virus::id_type const &parent_id);
  void create(Virus::id_type const &id, std::vector<Virus::id_type> const &parent_ids);

  // Dodaje nową krawędź w grafie genealogii.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
  void connect(Virus::id_type const &child_id, virus::id_type const &parent_id);

  // Usuwa wirus o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  // Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
  // wirusa macierzystego.
  void remove(Virus::id_type const &id);
}
#endif
