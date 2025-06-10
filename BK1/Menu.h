#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <string>

#define MAX_NUMBER_OF_ITEMS 5

class Menu {
private:
    int selectedItemIndex;                        // Mục menu đang được chọn
    int numberOfItems;                            // Số lượng mục menu thực tế
    sf::Font font;                                // Font chữ
    sf::Text menu[MAX_NUMBER_OF_ITEMS];           // Các mục menu

    sf::RectangleShape background;                // Hình nền (có thể dùng texture hoặc màu)
    sf::Texture backgroundTexture;                // Texture nền
    bool useTexture;                              // Cờ kiểm tra có dùng texture không

public:
    Menu(float width, float height);
    ~Menu();

    void draw(sf::RenderWindow& window);          // Vẽ menu
    void MoveUp();                                // Di chuyển lên
    void MoveDown();                              // Di chuyển xuống           // Đặt lại mục đang chọn

    int getSelectedIndex() const;

    void setNumberOfItems(int num);               // Thiết lập số mục menu sử dụng thực tế
    void setMenuItemString(int index, const std::string& str);  // Đổi nội dung mục menu

    void updateMenuPositions(float width);        // Cập nhật lại vị trí menu chính
    void updateGameOverMenuPositions(float width);// Cập nhật lại vị trí menu Game Over
};

#endif // MENU_H
