#include <iostream>
#include <memory>
#include <utility>

class Vehicle {
private:
  class Interface {
  public:
    virtual ~Interface()                             = default;
    virtual void                       drive()       = 0;
    virtual std::unique_ptr<Interface> clone() const = 0;
  };

  template<typename ImplementationType>
  class Delegator : public Interface {
  public:
    explicit Delegator(const ImplementationType& implementation)
      : m_implementation{implementation}
    {
    }

    explicit Delegator(ImplementationType&& implementation)
      : m_implementation{std::move(implementation)}
    {
    }

    void drive() override
    {
      m_implementation.drive();
    }

    std::unique_ptr<Interface> clone() const override
    {
      return std::make_unique<Delegator>(*this);
    }

  private:
    ImplementationType m_implementation;
  };

public:
  template<typename ImplementationType>
  explicit Vehicle(ImplementationType&& implementation)
    : m_vehicle{new Delegator<ImplementationType>{
      std::forward<ImplementationType>(implementation)}}
  {
  }

  Vehicle(const Vehicle& other) : m_vehicle{nullptr}
  {
    if (other.m_vehicle != nullptr) {
      m_vehicle = other.m_vehicle->clone();
    }
  }

  Vehicle(Vehicle&& other) noexcept = default;

  Vehicle& operator=(const Vehicle& other)
  {
    if (other.m_vehicle != nullptr) {
      m_vehicle = other.m_vehicle->clone();
    }
    else {
      m_vehicle = nullptr;
    }

    return *this;
  }

  Vehicle& operator=(Vehicle&& other) noexcept = default;

  void drive()
  {
    m_vehicle->drive();
  }

private:
  std::unique_ptr<Interface> m_vehicle;
};

class Car {
public:
  void drive()
  {
    std::cout << "Vroom, vroom. I'm a car!\n";
  }
};

class Truck {
public:
  void drive()
  {
    std::cout << "VROOOM. I'm a truck!\n";
  }
};

int main()
{
  Vehicle vehicle{Car{}};
  vehicle.drive();

  vehicle = Vehicle{Truck{}};
  vehicle.drive();
}
