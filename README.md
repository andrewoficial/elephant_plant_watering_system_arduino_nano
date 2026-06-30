# ElephantPlantWatering

Описание
elephant_plant_watering_system_arduino_nano — это прошивка для исполнительного устройства (конечного узла/ноды) распределенной системы умного мониторинга и полива растений. <br>
Данное приложение работает в связке с главным контроллером (ESP32), который предоставляет Web-UI и управляет всей сетью. <br>
Задача этого устройства — сбор данных с датчиков микроклимата и почвы, а также физическое управление поливом (реле насоса). <br>
Устройство поддерживает два варианта радиомодулей для связи с шлюзом: HC-12 (UART/433MHz) и RAK811 (LoRa), что позволяет гибко настраивать сеть в зависимости от расстояния и условий.<br><br>

Поддерживаемые функции и периферия:<br>
    Измерение температуры и влажности воздуха (DHT11/DHT22).<br>
    Измерение влажности почвы (аналоговый/цифровой датчик).<br>
    Управление водяным насосом через релейный модуль.<br>
    Измерение температуры почвы (в планах/в разработке).<br>
    Двухвариантная реализация радиоканала (переключение между HC-12 и RAK811).<br><br>

Технологии и оборудование<br>
В данном проекте были использованы следующие технологии, библиотеки и компоненты:<br>

    C++ / Arduino Framework: Основной язык и фреймворк для написания прошивки.<br>
    PlatformIO: Среда и система сборки для управления зависимостями, компиляции и прошивки микроконтроллера.<br>
    Adafruit DHT sensor library: Библиотека для опроса датчиков температуры и влажности воздуха.<br>
    Adafruit Unified Sensor: Базовая библиотека для унификации работы с сенсорами Adafruit.<br>
    Микроконтроллер: Arduino Nano (ATmega328).<br>
    Радиомодули: HC-12 или RAK811 (LoRa) для связи с ESP32.<br><br>

Сборка и запуск<br>
Для успешной сборки и прошивки устройства Вам понадобится:<br>

    Visual Studio Code с установленным расширением PlatformIO IDE.<br>
    Все зависимости уже указаны в файле platformio.ini и подтянутся автоматически.<br>
    Подключенная плата Arduino Nano (в конфиге указан порт COM13, при необходимости измените upload_port и monitor_port в platformio.ini на ваш актуальный порт).<br>

# Для открытия монитора порта (скорость 9600 бод):<br>
pio device monitor<br>


Примечание: Если ваш аппаратный вариант использует специфические настройки для радиомодулей (HC-12 или RAK811), вы 
можете раскомментировать или добавить соответствующие build_flags в файл platformio.ini (например, -DUSE_LORA или -DUSE_HC12), если это предусмотрено логикой кода.<br>

    

## Лицензия

##### CC BY-NC 4.0 в следующей нотации:
  ###### RU
     Creative Commons Attribution-NonCommercial 4.0 Международная общедоступная лицензия
     
     Осуществляя Лицензионные права (определенные ниже), Вы принимаете и соглашаетесь соблюдать положения и условия настоящей публичной лицензии Creative Commons Attribution-NonCommercial 4.0 International ("Публичная лицензия"). В той мере, в какой эта Публичная лицензия может быть истолкована как договор, Вам предоставляются Лицензионные права при условии, что Вы принимаете настоящие положения и условия, а Лицензиар предоставляет Вам такие права с учетом выгод, которые Лицензиар получает от предоставления Лицензируемых материалов. на этих условиях.
    
     Вы можете:
     - Распространять — копируйте и распространяйте материал на любом носителе и в любом формате
     - Адаптировать — изменять, адаптировать и создавать на основе 
     
     На следующих условиях:
     - Авторство — вы должны предоставить ссылку на лицензию и указать, ссылку на репозиторий проекта, были ли внесены изменения. Вы можете сделать это любым разумным способом, но никоим образом не предполагающим, что лицензиар одобряет вас или ваше использование.
     - NonCommercial — Вы не можете использовать материал в коммерческих целях.
     
  ###### EN
    Creative Commons Attribution-NonCommercial 4.0 International Public License
    
    By exercising the Licensed Rights (defined below), You accept and agree to be bound by the terms and conditions of this Creative Commons Attribution-NonCommercial 4.0 International Public License ("Public License"). To the extent this Public License may be interpreted as a contract, You are granted the Licensed Rights in consideration of Your acceptance of these terms and conditions, and the Licensor grants You such rights in consideration of benefits the Licensor receives from making the Licensed Material available under these terms and conditions.
    
    You are free to:
    - Share — copy and redistribute the material in any medium or format
    - Adapt — remix, transform, and build upon the material
    
    Under the following terms:
    - Attribution — You must give appropriate credit, provide a link to the license,link to the github page project and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
    - NonCommercial — You may not use the material for commercial purposes.

## Ответственность
###### RU
    Программный продукт, представленный в этом репозитории, предоставляется "как есть" без каких-либо явных или подразумеваемых гарантий, включая, но не ограничиваясь, подразумеваемыми гарантиями коммерческой ценности, пригодности для конкретной цели и невыполнения прав. 
    Разработчик не несет ответственности за любые проблемы, ошибки или неполадки, возникшие при использовании данного продукта. Использование продукта осуществляется на ваш собственный риск.
      
###### EN
    The software product provided in this repository is provided "as is" without warranty of any kind, either express or implied, including, but not limited to, the implied warranties of merchantability, fitness for a particular purpose, and non-infringement.
    The developer is not responsible for any problems, errors or malfunctions that occur when using this product. Use of the product is at your own risk.

## Обратная связь
Ниже найдете список ссылок для связи с автором.

| Платформа     | Ссылка                                                                    | Отвечу за |
| ------------- |:-------------------------------------------------------------------------:| --------- |
| Почта         | [Ссылка](mailto:andrewoficial@yandex.ru "Ссылка")                         | 24 часа   |
| LinkedIn      | [Ссылка](https://www.linkedin.com/in/andrey-kantser-126554258/ "Ссылка")  | 3 часа    |
| Telegram      | [Ссылка](https://t.me/function_void "Ссылка")                             | 5 минут   |
