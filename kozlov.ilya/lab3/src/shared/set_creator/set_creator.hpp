
template<typename T>
const std::string SetCreator<T>::tag = "SetCreator";

template<typename T>
Set<T>* SetCreator<T>::get(const Type& type)
{
  switch (type)
  {
  case Type::OPTIMISTIC:
  {
    Logger::logDebug(tag, "Constructing optimistic list...");
    Node<T>* head = NodeCreator<T>(MinValue::get<T>()).template get<Node<T>>();
    if (head != nullptr)
    {
      head->next = NodeCreator<T>(MaxValue::get<T>()).template get<Node<T>>();
      if (head->next != nullptr)
      {
        return new OptimisticList<T>(head);
      }
    }
    return nullptr;
  }
  case Type::LAZY:
  {
    Logger::logDebug(tag, "Constructing lazy list...");
    LazyNode<T>* head = NodeCreator<T>(MinValue::get<T>()).template get<LazyNode<T>>();
    if (head != nullptr)
    {
      head->next = NodeCreator<T>(MaxValue::get<T>()).template get<LazyNode<T>>();
      if (head->next != nullptr)
      {
        return new LazyList<T>(head);
      }
    }
    return nullptr;
  }
  }
  return nullptr;
}
