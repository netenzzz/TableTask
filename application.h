#include <gtk/gtk.h>
#include <gtkextra/gtksheet.h>
#include <string>
#include <vector>
class Application {
 public:
  Application();

 private:
  // There shold be different callbacks

  static void OpenButton(GtkButton *button, gpointer data);
  static void SaveButton(GtkButton *button, gpointer data);
  static void SheetActivatedRowCallback(GtkSheet *sheet, gint row,
                                        gpointer data);
  static void GenerateButtonCallback(GtkButton *button, gpointer data);
  static void FillTable(Application *data);
  void SetNumberOfRows(const int number_of_rows) {
    number_of_rows_ = number_of_rows;
  };

  void WriteTableToFile();
  void SetFilePath(const std::string path) { filepath_ = path; };
  const std::string GetFilePath() { return filepath_; };
  void SetTableFromFile();
  int GetNumberOfRows() { return number_of_rows_; };

 private:
  std::string filepath_;
  const std::vector<std::string> GenerateRandomRow();
  int number_of_rows_;
  GtkWidget *window_main_, *generate_button_, *entry_box_, *file_select_,
      *file_save_;
  GtkWidget *error_label_, *row_label_, *progress_label_;
  GtkWidget *vertical_packing_window_, *horisontal_packing_window_,
      *save_open_packing_window_;
  GtkWidget *scrolled_window_;
  GtkWidget *sheet_;
  bool counting_;
  Application *this_;
  bool created_, read_from_file_;
  std::vector<std::string> file_rows_;
};