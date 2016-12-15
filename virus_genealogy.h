#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

#include <memory>
#include <vector>
#include <set>
#include <map>

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

template <class Virus>
class VirusGenealogy {
private:
  //Klasa Virus reprezentuje informacje o wirusie. Jej implementacja
  //zostanie dostarczona w stosownym czasie.
  class VirusNode;

  typedef typename Virus::id_type id_type;
  typedef typename std::shared_ptr<VirusNode> node_ptr;
  typedef typename std::set<node_ptr> node_set;
  typedef typename std::map<id_type, node_ptr> node_map;
  typedef typename std::pair<id_type, node_ptr> pair_id_ptr;

  id_type stem_id;
  node_map viruses;

  class VirusNode {
  private:
    Virus virus;
    node_set children;
    node_set parents;

  public:
    VirusNode(id_type const &virus_id) : virus(Virus(virus_id)) {};

    VirusNode(id_type const &id, id_type const &parent_id) : virus(Virus(id)) {
      parents.insert(viruses.at(parent_id));
    }

    // TODO czy potrzebne
    VirusNode(id_type const &id, std::vector<id_type> const &parent_ids) : virus(Virus(id)) {
      for (id_type parent_id : parent_ids)
        parents.insert(viruses.at(parent_id));
    }

    Virus get_virus() {
      return virus;
    }

    void add_child(node_ptr child) {
      children.insert(child);
    }

    void add_parent(node_ptr parent) {
      parents.insert(parent);
    }

    void remove_child(node_ptr child) {
      children.erase(child);
    }

    void remove_parent(node_ptr parent) {
      parents.erase(parent);
    }

    std::vector<id_type> get_parents() {
      std::vector<id_type> parentsVec;
      for (auto parent : parents)
        parentsVec.push_back(parent);
      return parentsVec;
    }

    std::vector<id_type> get_children() {
      std::vector<id_type> childrenVec;
      for (auto child : children)
        childrenVec.push_back(child);
      return childrenVec;
    }
  };

public:
  //Klasa VirusGenealogy powinna udostępniać następujący interfejs.

  // Tworzy nową genealogię.
  // Tworzy także węzeł wirusa macierzystego o identyfikatorze stem_id.
  VirusGenealogy(id_type const &stem_id) : stemID(stem_id) {
    node_ptr root = std::make_shared<VirusNode>(stem_id); // TODO rzuca bad_alloc - jeżeli rzuci, to nic się nie stworzyło
    viruses.insert(pair_id_ptr(stem_id, root));
  };

  // Zwraca identyfikator wirusa macierzystego.
  id_type get_stem_id() const {
    return stem_id;
  }

  // Zwraca listę identyfikatorów bezpośrednich następników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<id_type> get_children(id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();

    try {
      return viruses.at(id).get_children();
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  // Zwraca listę identyfikatorów bezpośrednich poprzedników wirusa
  // o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli dany wirus nie istnieje.
  std::vector<id_type> get_parents(id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();

    try {
      return viruses.at(id).get_parents();
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  // Sprawdza, czy wirus o podanym identyfikatorze istnieje.
  bool exists(id_type const &id) const {
    return viruses.find(id) != viruses.end();
  }

  // Zwraca referencję do obiektu reprezentującego wirus o podanym
  // identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  Virus& operator[](id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();

    return *(viruses.at(id).get_virus());
  }

  // Tworzy węzeł reprezentujący nowy wirus o identyfikatorze id
  // powstały z wirusów o podanym identyfikatorze parent_id lub
  // podanych identyfikatorach parent_ids.
  // Zgłasza wyjątek VirusAlreadyCreated, jeśli wirus o identyfikatorze
  // id już istnieje.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z wyspecyfikowanych
  // poprzedników nie istnieje.
  void create(id_type const &id, id_type const &parent_id) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (!exists(parent_id))
      throw VirusNotFound();

    /* nie wiem czy nie lepiej jak na dole - i tak dla parent trzeba dodać child
    node_ptr new_virus = node_ptr(VirusNode(id, parent_id));
    viruses.insert(pair_id_ptr(id, new_virus));
    */
    // TODO połapać wyjątki
    node_ptr new_virus = std::make_shared<Node>(id);
    node_ptr parent    = viruses.at(parent_id);

    new_virus.add_parent(parent);
    parent.add_child(new_virus);
    viruses.insert(pair_id_ptr(id, new_virus));
  }


  void create(id_type const &id, std::vector<id_type> const &parent_ids) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (parent_ids.empty())
      throw VirusNotFound();
    for (int parent : parents_id) {
      if (!exists(parent))
        throw VirusNotFound();
    }

    // TODO wyjatki
    node_ptr new_virus = std::make_shared<Node>(id);
    node_ptr parent;
    viruses.insert(pair_id_ptr(id, new_virus));
    for (int parent_id : parents_id) {
      parent = viruses.at(parent_id);
      new_virus.add_parent(parent);
      parent.add_child(new_virus);
    }
  }


  // Dodaje nową krawędź w grafie genealogii.
  // Zgłasza wyjątek VirusNotFound, jeśli któryś z podanych wirusów nie istnieje.
  void connect(id_type const &child_id, id_type const &parent_id) {
    if (!exists(parent_id) || !exists(child_id))
      throw VirusNotFound();

    node_ptr child = viruses.at(child_id);
    node_ptr parent = viruses.at(parent_id);
    child.add_parent(parent);
    parent.add_child(child);
  }

  // Usuwa wirus o podanym identyfikatorze.
  // Zgłasza wyjątek VirusNotFound, jeśli żądany wirus nie istnieje.
  // Zgłasza wyjątek TriedToRemoveStemVirus przy próbie usunięcia
  // wirusa macierzystego.
  void remove(id_type const &id) {
    if (!exists(id))
      throw VirusNotFound();
    if (id == stemID)
      throw TriedToRemoveStemVirus();

    node_ptr virus = viruses.at(id);
    node_ptr parent;
    node_ptr child;
    std::vector<int> parents = virus.get_parents();
    std::vector<int> children = virus.get_children();

    for (int parent_id : parents) {
      parent = viruses.at(parent_id);
      parent.remove_child(virus);
    }

    for (int child_id : children) {
      child = viruses.at(child_id);
      child.remove_parent(virus);
      // TODO albo tu albo w remove_parent trzeba chyba usuwac jak nie ma parentow
    }

    viruses.erase(id);
  }
};

#endif //VIRUS_GENEALOGY_H
