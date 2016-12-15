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
    VirusNode(id_type const &virus_id) : virus(Virus(virus_id)) {}

    Virus& get_virus() {
      return virus;
    }

    bool has_parent() {
      return !parents.empty();
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
      std::vector<id_type> parents_vec;
      for (auto parent : parents)
        parents_vec.push_back(parent->get_virus().get_id());
      return parents_vec;
    }

    std::vector<id_type> get_children() {
      std::vector<id_type> children_vec;
      for (auto child : children)
        children_vec.push_back(child->get_virus().get_id());
      return children_vec;
    }
  };

  VirusGenealogy& operator=(const VirusGenealogy& that) {}
  VirusGenealogy& operator=(const VirusGenealogy&& that) {}
  VirusGenealogy(const VirusGenealogy& that) {}
  VirusGenealogy(const VirusGenealogy&& that) {}

public:

  VirusGenealogy(id_type const &stem_id) : stem_id(stem_id) {
    node_ptr root = std::make_shared<VirusNode>(stem_id); // TODO rzuca bad_alloc - jeżeli rzuci, to nic się nie stworzyło
    viruses.insert(pair_id_ptr(stem_id, root));
  }

  id_type get_stem_id() const {
    return stem_id;
  }

  std::vector<id_type> get_children(id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();

    try {
      return viruses.at(id)->get_children();
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  std::vector<id_type> get_parents(id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();

    try {
      return viruses.at(id)->get_parents();
    } catch (...) {
      return std::vector<id_type>();
    }
  }

  bool exists(id_type const &id) const {
    return viruses.find(id) != viruses.end();
  }

  Virus& operator[](id_type const &id) const {
    if (!exists(id))
      throw VirusNotFound();
    return viruses.at(id)->get_virus();
  }

  void create(id_type const &id, id_type const &parent_id) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (!exists(parent_id))
      throw VirusNotFound();

    // TODO połapać wyjątki
    node_ptr new_virus = std::make_shared<VirusNode>(id);
    node_ptr parent    = viruses.at(parent_id);

    new_virus->add_parent(parent);
    parent->add_child(new_virus);
    viruses.insert(pair_id_ptr(id, new_virus));
  }

  void create(id_type const &id, std::vector<id_type> const &parent_ids) {
    if (exists(id))
      throw VirusAlreadyCreated();
    if (parent_ids.empty())
      throw VirusNotFound();
    for (id_type parent : parent_ids) {
      if (!exists(parent))
        throw VirusNotFound();
    }

    // TODO wyjatki
    node_ptr new_virus = std::make_shared<VirusNode>(id);
    node_ptr parent;
    viruses.insert(pair_id_ptr(id, new_virus));
    for (id_type parent_id : parent_ids) {
      parent = viruses.at(parent_id);
      new_virus->add_parent(parent);
      parent->add_child(new_virus);
    }
  }

  void connect(id_type const &child_id, id_type const &parent_id) {
    if (!exists(parent_id) || !exists(child_id))
      throw VirusNotFound();

    // TODO wyjatki
    node_ptr child  = viruses.at(child_id);
    node_ptr parent = viruses.at(parent_id);
    child->add_parent(parent);
    parent->add_child(child);
  }

  void remove(id_type const &id) {
    if (!exists(id))
      throw VirusNotFound();
    if (id == stem_id)
      throw TriedToRemoveStemVirus();

    // TODO wyjatki
    node_ptr virus = viruses.at(id);
    node_ptr parent;
    node_ptr child;
    std::vector<id_type> parents  = (*virus).get_parents();
    std::vector<id_type> children = (*virus).get_children();

    for (id_type parent_id : parents) {
      parent = viruses.at(parent_id);
      parent->remove_child(virus);
    }

    for (id_type child_id : children) {
      child = viruses.at(child_id);
      child->remove_parent(virus);
    }

    viruses.erase(id);

    for (id_type child_id : children) {
      child = viruses.at(child_id);
      if (!child->has_parent() && child_id != stem_id)
        remove(child_id);
    }
  }
};

#endif //VIRUS_GENEALOGY_H
