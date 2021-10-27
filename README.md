# About
Xml writer with Variadic CRTP based extension system. The Writer class itself offers very basic xml writing functions like opening/closing elements, appending attributes and inline text. More specialized interface can be added as extension classes using template arguments as seen in examples. This method was choosen over simpler solutions (like standard inheritance) too allow methods chaining with interleaving functions from different extensions and basic class. In other words both basic class and extensions are aware of eachother.

# Requirements
C++17 compatible compiler and fmt library.