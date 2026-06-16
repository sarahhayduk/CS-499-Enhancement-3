#ifndef MENU_H
#define MENU_H

#include <vector>
#include "Database.h"

/*---------------------------------------------------------------------------------
 * Menu.h
 *
 * This header defines the public interface for all menu-related operations by the
 * program. It exposes the functions responsible for displaying the menu and menu
 * options: 1 - DisplayInfo, 2 - ChangeCustomerChoice, 3 - AddNewClient.
 * -------------------------------------------------------------------------------*/
void DisplayInfo(const std::vector<Client>& clients);
void ChangeCustomerChoice(std::vector<Client>& clients);
void AddNewClient(std::vector<Client>& clients);
void DisplayMenu(std::vector<Client>& clients);

#endif