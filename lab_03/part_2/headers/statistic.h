#pragma once

#include<string>

#pragma once

#include<vector>
#include<string>
#include <map>
#include <fstream>
#include <stdio.h>
#include <errno.h>

// II. Сохранять статистику о различных форматах файлов,
//  к которым обращался тот или иной пользователь

class Statistics 
{
private: 
    std::string filename;
    std::map<std::string, std::vector<std::string>> _statistics;

public: 
    Statistics(const char *filename);

    void set_statistics(std::string extent, std::string client); 
    void write_in_file(const char*  filename);
    void read_from_file(const char*  filename);
};

Statistics::Statistics(const char *filename) 
{
	this->read_from_file(filename);
	this->filename = filename;
}

void Statistics::set_statistics(std::string extent, std::string client) 
{
	// Считываем статистику из файла.
	this->read_from_file(this->filename.c_str());

	// Добавляем информацию, о запросе нового расширения.
	std::vector<std::string> _statistics = this->_statistics[extent];
	_statistics.push_back(client);
	this->_statistics[extent] = _statistics;

	// Сохраняем статитику. 
	this->write_in_file(this->filename.c_str());
}

// Записывает в файл статистику.
void Statistics::write_in_file(const char*  filename) 
{
	std::ofstream file(filename);

	std::map<std::string, std::vector<std::string>> copy = this->_statistics;

	for (std::pair<std::string, std::vector<std::string>> element : copy) 
	{
		// Расширение файла
		std::string exten = element.first;
		
 		// Кто запрашивал данное расширение.
		std::vector<std::string> clients = element.second;
		file << exten << std::endl;
		for (int i = 0; i < clients.size(); i++) 
		{
			file << clients[i] << std::endl;
		}
		file << std::endl;
	}
	
	file.close();
}

// Считывает из файла статистику.
void Statistics::read_from_file(const char*  filename) 
{
	std::ifstream file(filename);
	bool is_new_exten = true; 
	std::string exten;

	for(std::string line; std::getline(file, line); )
	{
		if (line == "") 
		{
			is_new_exten = true;
		} 
		else if (is_new_exten) 
		{
			exten = line;
			is_new_exten=false;
		} 
		else 
		{
			this->_statistics[exten].push_back(line);
		}
	} 

	file.close();
}
