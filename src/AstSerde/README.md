# AstSerde 序列化模块

## 概述

AstSerde 是一个跨语言的序列化框架，基于运行时反射机制，支持将 C++ 对象序列化为多种格式，包括：

- JSON
- XML
- C++ 代码
- Java 代码
- Python 代码

同时也支持从这些格式反序列化回 C++ 对象。

## 设计思路

### 核心组件

1. **Serializer 基类**：定义了序列化和反序列化的接口
2. **具体序列化器**：实现了不同格式的序列化和反序列化
   - JsonSerializer
   - XmlSerializer
   - CppSerializer
   - JavaSerializer
   - PythonSerializer
3. **SerializerFactory**：工厂类，用于创建不同类型的序列化器
4. **SerializationUtils**：工具类，提供便捷的序列化和反序列化方法

### 工作原理

1. **运行时反射**：利用 Object 类的运行时类型信息，获取对象的属性和类型
2. **属性遍历**：遍历对象的所有属性，根据属性类型进行序列化
3. **格式转换**：将对象的属性值转换为目标格式的表示
4. **代码生成**：对于 C++、Java、Python 格式，生成相应的代码

## 使用方法

### 基本用法

```cpp
#include "AstSerde.hpp"
#include "AstCore/Object.hpp"

// 序列化对象到 JSON
std::string jsonOutput;
err_t result = SerializationUtils::serialize(object, SerializationFormat::JSON, jsonOutput);

// 序列化对象到 C++ 代码
std::string cppCode;
result = SerializationUtils::serialize(object, SerializationFormat::Cpp, cppCode);

// 从 JSON 反序列化对象
std::string jsonInput = "{...}";
result = SerializationUtils::deserialize(jsonInput, SerializationFormat::JSON, object);
```

### API 函数

- `aObjectToCppCode`：将对象转换为 C++ 代码
- `aObjectSerialize`：将对象序列化为指定格式
- `aObjectDeserialize`：从指定格式反序列化对象

## 扩展方法

要添加新的序列化格式，只需：

1. 创建一个继承自 `Serializer` 的新类
2. 实现 `serialize` 和 `deserialize` 方法
3. 在 `SerializerFactory::createSerializer` 中添加新的格式支持

## 依赖关系

- AstUtil：提供运行时反射机制
- AstCore：提供 Object 类定义

## 示例

### 序列化到 JSON

```cpp
// 假设有一个名为 Person 的类，有 name 和 age 属性
Person person;
person.setAttrString("name", "Alice");
person.setAttrInt("age", 30);

std::string jsonOutput;
SerializationUtils::serialize(&person, SerializationFormat::JSON, jsonOutput);
// 输出: {"__type__":"Person","name":"Alice","age":30}
```

### 序列化到 C++ 代码

```cpp
std::string cppCode;
SerializationUtils::serialize(&person, SerializationFormat::Cpp, cppCode);
// 生成的代码:
// #include "AstCore/Object.hpp"
// #include <memory>
// #include <string>
// 
// using namespace ast;
// 
// std::shared_ptr<Person> createPerson() {
//     auto obj = std::make_shared<Person>();
//     obj->setAttrString("name", "Alice");
//     obj->setAttrInt("age", 30);
//     return obj;
// }
```

### 序列化到 Java 代码

```cpp
std::string javaCode;
SerializationUtils::serialize(&person, SerializationFormat::Java, javaCode);
// 生成的代码:
// public class Person {
//     private String name;
//     private int age;
//     
//     public Person() {
//         this.name = "Alice";
//         this.age = 30;
//     }
//     
//     public String getName() {
//         return name;
//     }
//     
//     public void setName(String name) {
//         this.name = name;
//     }
//     
//     public int getAge() {
//         return age;
//     }
//     
//     public void setAge(int age) {
//         this.age = age;
//     }
// }
```

### 序列化到 Python 代码

```cpp
std::string pythonCode;
SerializationUtils::serialize(&person, SerializationFormat::Python, pythonCode);
// 生成的代码:
// class Person:
//     def __init__(self):
//         self.name = "Alice"
//         self.age = 30
//     
//     def to_dict(self):
//         return {
//             'name': self.name,
//             'age': self.age
//         }
//     
//     @classmethod
//     def from_dict(cls, data):
//         obj = cls()
//         if 'name' in data:
//             obj.name = data['name']
//         if 'age' in data:
//             obj.age = data['age']
//         return obj
```