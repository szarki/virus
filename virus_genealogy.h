#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

class VirusAlreadyCreated : public std::exception {
  virtual const char* what() const throw() {
    return "VirusAlreadyCreated";
  }
};

class VirusNotFound : public std::exception {
  virtual const char* what() const throw() {
    return "VirusNotFound";
  }
};

class TriedToRemoveStemVirus : public std::exception {
  virtual const char* what() const throw() {
    return "TriedToRemoveStemVirus";
  }
};

template <class Virus> class VirusGenealogy {
private:
  //Klasa Virus reprezentuje informacje o wirusie. Jej implementacja
  //zostanie dostarczona w stosownym czasie.

  typedef typename Virus::id_type id_type;

  id_type stemID;
  std::map<id_type, std::vector<id_type>> children;
  std::map<id_type, std::vector<id_type>> parents;
  std::vector<id_type> viruses;

public:
  //Klasa VirusGenealogy powinna udostępniać następujący interfejs.

  // Tworzy nową genealogię.
  // Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
  VirusGenealogy(id_type const &stem_id) : stemID(stem_id) {};

  // Zwraca identyfikator wirusa macierzystego.
  id_type get_stem_id() const {
    return stemID;
  }

  // Zwraca listę identyfikatorów bezpośrednich następników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<id_type> get_children(Virus::id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();
    try {
      return children.at(id);
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  // Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<Virus::id_type> get_parents(Virus::id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();
    try {
      return parents.at(id);
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  // Sprawdza, czy wirus o podanym identyfikatorze istnieje.
  bool exists(Virus::id_type const &id) const {
    return viruses.find(id) != viruses.end();
  }

  // Zwraca referencję do obiektu reprezentującego wirus o podanym
  // identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  Virus& operator[](Virus::id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();
    return viruses.at(id);
  }

  // Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
  // powstały z wirusów o podanym identyfikatorze parent_id lub
  // podanych identyfikatorach parent_ids.
  // Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
  // id już istnieje.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
  // poprzedników nie istnieje.
  void create(Virus::id_type const &id, Virus::id_type const &parent_id) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (!exists(parent_id))
      throw VirusNotFound();
    Virus* toAdd = new Virus(id);
    viruses.insert(id, toAdd);
    children.insert(parent_id, id);
    parents.insert(id, parent_id);
  }

  void create(Virus::id_type const &id, std::vector<Virus::id_type> const &parent_ids) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (parent_ids.empty())
      throw VirusNotFound();
    for (int i=0; i<parent_ids.size(); ++i) {
      if (!exists(parent_ids[i]))
        throw VirusNotFound();
    }
    Virus* toAdd = new Virus(id);
    viruses.insert(id, toAdd);
    for (int i=0; i<parent_ids.size(); ++i) {
      children.insert(parent_ids[i], id);
      parents.insert(id, parent_ids[i]);
    }
  }

  // Dodaje nową krawędź w grafie genealogii.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
  void connect(Virus::id_type const &child_id, virus::id_type const &parent_id) {

  }

  // Usuwa wirus o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  // Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
  // wirusa macierzystego.
  void remove(Virus::id_type const &id) {
    if (!exists(id))
      throw VirusNotFound();
    if (id == stemID)
      throw TriedToRemoveStemVirus();
    viruses.erase(id);
    for (id_type it : parents.at(id))
      children[it].erase(id);
    for (id_type it : children.at(id)) {
      parents[it].erase(id);
      if (parents[it].empty())
        remove(it);
    }
  }
};
#endif
