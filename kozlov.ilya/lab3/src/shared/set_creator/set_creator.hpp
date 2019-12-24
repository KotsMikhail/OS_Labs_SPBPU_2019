
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
    typename OptimisticList<T>::Node* head = OptimisticList<T>::Node::create(MinValue::get<T>());
    Collector<typename OptimisticList<T>::Node> collector;
    if (head != nullptr && collector.init())
    {
      head->next = OptimisticList<T>::Node::create(MaxValue::get<T>());
      if (head->next != nullptr)
      {
        return new OptimisticList<T>(head, collector);
      }
    }
    return nullptr;
  }
  case Type::LAZY:
  {
    Logger::logDebug(tag, "Constructing lazy list...");
    typename LazyList<T>::LazyNode* head = LazyList<T>::LazyNode::create(MinValue::get<T>());
    Collector<typename LazyList<T>::LazyNode> collector;
    if (head != nullptr && collector.init())
    {
      head->next = LazyList<T>::LazyNode::create(MaxValue::get<T>());
      if (head->next != nullptr)
      {
        return new LazyList<T>(head, collector);
      }
    }
    return nullptr;
  }
  }
  return nullptr;
}
