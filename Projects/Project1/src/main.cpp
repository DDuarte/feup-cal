#include <iostream>
#include <memory>
#include <functional>
#include <fstream>
#include <thread>

#include "utils.h"
#include "hydrographic_network.h"
#include "menu.h"
#include "loader.h"
#include "consolereader.h"
#include "delivery.h"

#define MENU_SAVE_FILE "mainMenu.txt"
#define HYDROGRAPHIC_NETWORK_SAVE_FILES_PREFIX "hydro_"
#define ORDER_SAVE_FILES_PREFIX "order_"

// Main menu
void NewHydrographicBasin();
void LoadHydrographicBasin();

// HydrographicNetwork menu
void HydrographicNetworkMenu(HydrographicNetwork* hn);
void AddVillage(HydrographicNetwork* hn);
void AddRiver(HydrographicNetwork* hn);
void ChangeSeason(HydrographicNetwork* hn);
void ViewHydrographicBasin(HydrographicNetwork* hn);
void ViewDelivery(HydrographicNetwork* hn);
void NewDelivery(HydrographicNetwork* hn);
void SaveHydrographicNetwork(HydrographicNetwork* hn);

// Delivery menu
void NewOrder(HydrographicNetwork* hn, Delivery* d);
void SetSourceVillage(HydrographicNetwork* hn, Delivery* d);
void SetBoatCapacity(HydrographicNetwork* hn, Delivery* d);
void SetSupportVesselCapacity(HydrographicNetwork* hn, Delivery* d);
void SetNumberOfSupportVessels(HydrographicNetwork* hn, Delivery* d);
void SaveDelivery(HydrographicNetwork* hn, Delivery* d);

int main(int argc, char** argv)
{
    std::unique_ptr<Menu> menu(Loader<Menu>(MENU_SAVE_FILE).Load());
    if (!menu)
    {
        std::cerr << MENU_SAVE_FILE << " could not be loaded." << std::endl;
        return EXIT_FAILURE;
    }

    bool executing = true;

    std::function<void(void)> functions[] =
    {
        [&executing]() { executing = false; },  // 1 - exit
        NewHydrographicBasin,                   // 2
        LoadHydrographicBasin                   // 3
    };

    while (executing)
    {
        try
        {
            uint32 option = menu->Print();
            functions[option == 0 ? 0 : option - 1]();
        }
        catch (ActionCanceled& action)
        {
            PauseConsole(std::string(action.what()) + " canceled...\nPress enter to continue...");
            ClearConsole();
            continue;
        }
        catch (std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
            PauseConsole();
            ClearConsole();
        }
    }

    return EXIT_SUCCESS;
}

void HydrographicNetworkMenu(HydrographicNetwork* hn)
{
    bool success = false;

    try
    {
        do
        {
            static std::function<void(HydrographicNetwork*)> functions[] =
            {
                [&success](HydrographicNetwork*) { success = true; }, // 1 - exit
                AddVillage,             // 2
                AddRiver,               // 3
                ChangeSeason,           // 4
                ViewHydrographicBasin,  // 5
                ViewDelivery,           // 6
                NewDelivery,            // 7
                SaveHydrographicNetwork, // 8
            };

            try
            {
                uint32 option = HydrographicNetwork::GetMenu()->Print();
                functions[option == 0 ? 0 : option - 1](hn);
            }
            catch (ActionCanceled& action)
            {
                PauseConsole(std::string(action.what()) + " canceled...\nPress enter to continue...");
                ClearConsole();
                continue;
            }
            catch (std::runtime_error& e)
            {
                std::cerr << e.what() << std::endl;
                PauseConsole();
                ClearConsole();
            }
        } while (!success);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Hydrographic Basin");
    }

    if (hn->GetGraphViewer() != nullptr)
        hn->GetGraphViewer()->closeWindow();
}

void NewHydrographicBasin()
{
    std::string name;
    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", [](const std::string& name)
        {
            if (!NamePredicate(name))
                return false;

            std::string fileName = HYDROGRAPHIC_NETWORK_SAVE_FILES_PREFIX + name + ".txt";
            fileName = to_lower(fileName);

            if (File::Exists(fileName.c_str()))
            {
                std::cout << "A Hydrographic Network with the same name already exists." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Hydrographic Basin");
    }

    ClearConsole();

    std::unique_ptr<HydrographicNetwork> hn(new HydrographicNetwork(name));
    HydrographicNetworkMenu(hn.get());
}

void LoadHydrographicBasin()
{
    std::string prefix = HYDROGRAPHIC_NETWORK_SAVE_FILES_PREFIX;
    std::string suffix = ".txt";

    std::vector<std::string> fileNames;
    File::GetFiles(".", fileNames, [&prefix](const std::string& str)
    {
        return starts_with(str, prefix);
    });

    if (fileNames.empty())
    {
        std::cout << "Could not find any hydrographic basin to view. Create some first?" << std::endl;
        return;
    }
    else
    {
        std::cout << "Available hydrographic basins:" << std::endl;
        for (const std::string& name : fileNames)
            std::cout << "\t - " << name.substr(prefix.size(), name.size() - suffix.size() - prefix.size()) << std::endl;
    }

    std::string selectedHydrographicBasinName;

    try
    {
        selectedHydrographicBasinName = ReadValue<std::string>("Hydrographic basin name: ", [&fileNames, &prefix, &suffix](const std::string& val)
        {
            if (std::find(fileNames.begin(), fileNames.end(), prefix + val + suffix) == fileNames.end())
            {
                std::cout << "Specified hydrographic basin does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Load Hydrographic Basin");
    }

    ClearConsole();

    std::unique_ptr<HydrographicNetwork> hn(std::unique_ptr<HydrographicNetwork>(Loader<HydrographicNetwork>(prefix + selectedHydrographicBasinName + suffix).Load()));
    HydrographicNetworkMenu(hn.get());
}

void AddVillage(HydrographicNetwork* hn)
{
    std::string name;
    double x, y;

    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", NamePredicate);

        x = ReadValue<double>("Position X [>0]: ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "X position cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        y = ReadValue<double>("Position Y [>0]: ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Y position cannot be negative." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Add Village");
    }

    uint id = hn->AddVillage(Village(name, x, y));

    std::cout << "Village created, assigned id " << id << "." << std::endl;

    if (hn->GetGraphViewer())
        hn->GetGraphViewer()->rearrange();

    PauseConsole();
    ClearConsole();
}

void AddRiver(HydrographicNetwork* hn)
{
    std::string name;
    uint sourceVillage, destVillage;
    uint maxCapacity;

    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", NamePredicate);

        sourceVillage = ReadValue<uint>("Source village id: ", [hn](uint val)
        {
            if (hn->GetVillage(val) == NULL)
            {
                std::cout << "Village with the specified id does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        destVillage = ReadValue<uint>("Destination village id: ", [hn](uint val)
        {
            if (hn->GetVillage(val) == NULL)
            {
                std::cout << "Village with the specified id does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        maxCapacity = ReadValue<uint>("Max capacity [>0]: ");
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Add River");
    }

    uint id = hn->AddRiver(sourceVillage, destVillage, River(name, maxCapacity));

    std::cout << "River created, assigned id " << id << "." << std::endl;

    if (hn->GetGraphViewer())
        hn->GetGraphViewer()->rearrange();

    PauseConsole();
    ClearConsole();
}

void ChangeSeason(HydrographicNetwork* hn)
{
    std::string season;

    try
    {
        season = ReadValue<std::string>("Season (summer, fall, winter, spring): ", [](const std::string& val)
        {
            std::string seasonName = to_lower(val);

            if (seasonName != "summer" && seasonName != "fall" && seasonName != "winter" && seasonName != "spring")
            {
                std::cout << "Specified season does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Change Season");
    }

    double factor = 1.0;
    if (season == "summer")
        factor = 0.5;
    else if (season == "fall")
        factor = 1.0;
    else if (season == "winter")
        factor = 1.5;
    else if (season == "spring")
        factor = 0.75;

    hn->ChangeRiversCapacity(factor);

    std::cout << "Rivers capacity factor changed to " << factor << "." << std::endl;

    if (hn->GetGraphViewer())
        hn->GetGraphViewer()->rearrange();

    PauseConsole();
    ClearConsole();
}

void ViewHydrographicBasin(HydrographicNetwork* hn)
{
    hn->ViewGraph();

    PauseConsole();
    ClearConsole();
}

void ViewDelivery(HydrographicNetwork* hn)
{
    std::string prefix = ORDER_SAVE_FILES_PREFIX + hn->GetName() + "_";
    std::string suffix = ".txt";

    std::vector<std::string> fileNames;
    File::GetFiles(".", fileNames, [hn, &prefix](const std::string& str)
    {
        return starts_with(str, prefix);
    });

    if (fileNames.empty())
    {
        std::cout << "Could not find any delivery to view. Create some first?" << std::endl;
        return;
    }
    else
    {
        std::cout << "Available deliveries:" << std::endl;
        for (const std::string& name : fileNames)
            std::cout << "\t - " << name.substr(prefix.size(), name.size() - suffix.size() - prefix.size()) << std::endl;
    }

    std::string selectedDeliveryName;

    try
    {
        selectedDeliveryName = ReadValue<std::string>("Delivery name: ", [&fileNames, &prefix, &suffix](const std::string& val)
        {
            if (std::find(fileNames.begin(), fileNames.end(), prefix + val + suffix) == fileNames.end())
            {
                std::cout << "Specified delivery does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("View Delivery");
    }

    //std::thread thread([hn, selectedDeliveryName]() // ViewGraph(delivery) blocks the caller due to silly animations
    //{
        Delivery* delivery = Loader<Delivery>(prefix + selectedDeliveryName + suffix).Load();
        DeliveryRoute deliveryRoute = hn->GetDeliveryPath(*delivery);
        std::cout << "Processing... Viewer windows is being updated." << std::endl;
        hn->ViewGraph(deliveryRoute);
        delete delivery;
    //});
    //thread.detach();

    PauseConsole();
    ClearConsole();
}

void NewDelivery(HydrographicNetwork* hn)
{
    std::string name;
    try
    {
        name = ReadValue<std::string>("Name (max 25 characters): ", [hn](const std::string& name)
        {
            if (!NamePredicate(name))
                return false;

            std::string fileName = ORDER_SAVE_FILES_PREFIX + hn->GetName() + "_" + name + ".txt";
            fileName = to_lower(fileName);

            if (File::Exists(fileName.c_str()))
            {
                std::cout << "A Delivery with the same name already exists." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Delivery");
    }

    ClearConsole();

    uint sourceVillage;
    double boatCapacity, supportVesselCapacity;
    uint numberOfSupportVessels;

    try
    {
        sourceVillage = ReadValue<uint>("Source village id: ", [hn](uint val)
        {
            if (hn->GetVillage(val) == NULL)
            {
                std::cout << "Village with the specified id does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        boatCapacity = ReadValue<double>("Boat capacity [>0]: ", [](double val)
        {
            if (val <= 0)
            {
                std::cout << "Boat capacity needs to be higher than 0." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        supportVesselCapacity = ReadValue<double>("Support vessel capacity [>0]: ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Support vessel capacity needs to be higher than 0." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        numberOfSupportVessels = ReadValue<uint>("Number of support vessel capacity [>=0]: ");
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Delivery");
    }

    PauseConsole("Delivery created with success. Press enter to continue...");
    ClearConsole();

    Delivery delivery(sourceVillage, boatCapacity, supportVesselCapacity, numberOfSupportVessels);

    delivery.SetName(name);

    bool success = false;

    try
    {
        do
        {
            static std::function<void(HydrographicNetwork*, Delivery*)> functions[] =
            {
                [&success](HydrographicNetwork*, Delivery*) { success = true; }, // 1 - exit
                NewOrder,                  // 2
                SetSourceVillage,          // 3
                SetBoatCapacity,           // 4
                SetSupportVesselCapacity,  // 5
                SetNumberOfSupportVessels, // 6
                SaveDelivery,              // 7
            };

            try
            {
                uint32 option = Delivery::GetMenu()->Print();
                functions[option == 0 ? 0 : option - 1](hn, &delivery);
            }
            catch (std::runtime_error& e)
            {
                std::cerr << e.what() << std::endl;
                PauseConsole();
                ClearConsole();
            }
        } while (!success);
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Delivery");
    }
}

void SaveHydrographicNetwork(HydrographicNetwork* hn)
{
    ByteBuffer bb(500);
    hn->Save(bb);

    std::string filename(HYDROGRAPHIC_NETWORK_SAVE_FILES_PREFIX + hn->GetName() + ".txt");

    if (!File::Save(filename.c_str(), bb, bb.Size()))
        std::cout << "An error occurred while saving to file " << filename << "." << std::endl;
    else
        std::cout << "File " << filename << " saved with success." << std::endl;

    PauseConsole();
    ClearConsole();
}

void NewOrder(HydrographicNetwork* hn, Delivery* d)
{
    uint weight, volume;
    uint destinationVillage;

    try
    {
        destinationVillage = ReadValue<uint>("Destination village id: ", [hn](uint val)
        {
            if (hn->GetVillage(val) == NULL)
            {
                std::cout << "Village with the specified id does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });

        weight = ReadValue<uint>("Weight [>0]: ");
        volume = ReadValue<uint>("Volume [>0]: ");
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("New Order");
    }

    d->AddOrder(destinationVillage, Order(weight, volume));

    std::cout << "New order created." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SetSourceVillage(HydrographicNetwork* hn, Delivery* d)
{
    uint sourceVillage;

    try
    {
        sourceVillage = ReadValue<uint>("Source village id: ", [hn](uint val)
        {
            if (hn->GetVillage(val) == NULL)
            {
                std::cout << "Village with the specified id does not exist." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Set Source Village");
    }

    d->SetSourceVillage(sourceVillage);

    std::cout << "Source village changed." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SetBoatCapacity(HydrographicNetwork* hn, Delivery* d)
{
    double boatCapacity;

    try
    {
        boatCapacity = ReadValue<double>("Boat capacity [>0]: ", [](double val)
        {
            if (val <= 0)
            {
                std::cout << "Boat capacity needs to be higher than 0." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Set Boat Capacity");
    }

    d->SetBoatCapacity(boatCapacity);

    std::cout << "Boat capacity changed." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SetSupportVesselCapacity(HydrographicNetwork* hn, Delivery* d)
{
    double supportVesselCapacity;

    try
    {
        supportVesselCapacity = ReadValue<double>("Support vessel capacity [>0]: ", [](double val)
        {
            if (val < 0)
            {
                std::cout << "Support vessel capacity needs to be higher than 0." << std::endl << "Please try again." << std::endl;
                return false;
            }

            return true;
        });
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Set Support Vessel Capacity");
    }

    d->SetSupportVesselCapacity(supportVesselCapacity);

    std::cout << "Support vessel capacity changed." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SetNumberOfSupportVessels(HydrographicNetwork* hn, Delivery* d)
{
    uint numberOfSupportVessels;

    try
    {
        numberOfSupportVessels = ReadValue<uint>("Number of support vessel [>=0]: ");
    }
    catch (EOFCharacterValue)
    {
        throw ActionCanceled("Set Number Of Support Vessels");
    }

    d->SetNumberOfSupportVessels(numberOfSupportVessels);

    std::cout << "Number of support vessels changed." << std::endl;

    PauseConsole();
    ClearConsole();
}

void SaveDelivery(HydrographicNetwork* hn, Delivery* d)
{
    ByteBuffer bb(500);
    d->Save(bb);

    std::string filename = ORDER_SAVE_FILES_PREFIX + hn->GetName() + "_" + d->GetName() + ".txt";

    if (!File::Save(filename.c_str(), bb, bb.Size()))
        std::cout << "An error occurred while saving to file " << filename << "." << std::endl;
    else
        std::cout << "File " << filename << " saved with success." << std::endl;

    PauseConsole();
    ClearConsole();
}
