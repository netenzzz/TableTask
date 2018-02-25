#include "application.h"
#include <gdk/gdk.h>
#include <cctype>
#include <climits>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <thread>

const char* column_names[] = {"Bool", "Integer", "Literal", "Float"};
const int columns_number = 4;
const int max_generated_int = 10000;
const char alphabet[] =
    "qwertyuiopasdfghjklzxcvbnm";  // Sorry for my literal random choosing

void Application::OpenButton(GtkButton* button, gpointer data) {
  GtkWidget* choosing_dialog = gtk_file_chooser_dialog_new(
      "Save File", GTK_WINDOW(((Application*)data)->window_main_),
      GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OK, GTK_RESPONSE_OK,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CLOSE, NULL);

  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(choosing_dialog), "/");
  gtk_widget_show_all(choosing_dialog);
  gtk_dialog_run(GTK_DIALOG(choosing_dialog));
  std::cout << gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(choosing_dialog))
            << std::endl;
  ((Application*)data)
      ->SetFilePath(
          gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(choosing_dialog)));
  ((Application*)data)->SetTableFromFile();
  gtk_widget_destroy (choosing_dialog);
};

void Application::SaveButton(GtkButton* button, gpointer data) {
  GtkWidget* choosing_dialog = gtk_file_chooser_dialog_new(
      "Save File", GTK_WINDOW(((Application*)data)->window_main_),
      GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_OK, GTK_RESPONSE_OK,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CLOSE, NULL);

  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(choosing_dialog), "/");
  gtk_widget_show_all(choosing_dialog);
  gtk_dialog_run(GTK_DIALOG(choosing_dialog));
  std::cout << gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(choosing_dialog))
            << std::endl;
  ((Application*)data)
      ->SetFilePath(
          gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(choosing_dialog)));
  ((Application*)data)->WriteTableToFile();
  gtk_widget_destroy (choosing_dialog);
};

void Application::SetTableFromFile() {
  std::ifstream file(filepath_);
  if (file.is_open()) {
    std::vector<std::string> rows;

    std::string row;
    while (std::getline(file, row)) {
      rows.push_back(row);
      row.clear();
    }
    FillTable(rows);
  }

}

void Application::WriteTableToFile() {
  std::ostringstream table;

  for (size_t i = 0; i < number_of_rows_; ++i) {
    for (size_t j = 0; j < columns_number; ++j)
      table << gtk_sheet_cell_get_text((GtkSheet*)sheet_, i, j) << " ";
    table << '\n';
  }

  std::ofstream file(filepath_, std::ofstream::out);
  if (file.is_open()) {
    std::cout << table.str();
    file << table.str();
  }
}

void Application::SheetActivatedRowCallback(GtkSheet* sheet, gint row,
                                            gpointer data) {
  std::ostringstream label_info;
  Application* app = (Application*)data;
  for (size_t i = 0; i < columns_number; ++i) {
    label_info << "Column " << (i + 1) << " : "
               << gtk_sheet_cell_get_text(sheet, row, i) << " \n";
  }

  gtk_label_set_text(((GtkLabel*)(app->row_label_)), label_info.str().c_str());
  gtk_widget_show(app->row_label_);
};

void Application::GenerateButtonCallback(GtkButton* button, gpointer data) {
  Application* app = (Application*)data;

  if (app->counting_) {
    app->counting_ = false;
  }

  std::string text = gtk_entry_get_text((GtkEntry*)app->entry_box_);

  bool normal_input = true;
  const int size = text.size();

  for (size_t i = 0; i < size; ++i) {
    if (isdigit(text[i]) != 0)
      gtk_widget_hide(app->error_label_);
    else {
      gtk_widget_show(app->error_label_);
      normal_input = false;
    }
  }
  int rows_number;
  long int checking_value = 0;
  if (normal_input) {
    std::istringstream str(text);
    str >> checking_value;
    if (checking_value > INT_MAX) {
      rows_number = 0;
      gtk_widget_show(app->error_label_);
    } else {
      rows_number = checking_value;
    }
    app->SetNumberOfRows(rows_number);
    if (app->calc_thread_.get_id() != std::thread::id()) {
      (app->calc_thread_).join();
    }

    // app->FillTable();
    // std::thread calc_thread(app->CallbackForThread,app->this_);
    std::thread calc_thread([data]() { ((Application*)data)->FillTable(); });
    std::cout << "Is joinable ? " << calc_thread.joinable() << std::endl;
    app->calc_thread_ = std::move(calc_thread);
    (app->calc_thread_).join();
  };
};

void Application::FillTable(const std::vector<std::string> if_from_file) {
  counting_ = true;
  const int last_rows_size = gtk_sheet_get_rows_count((GtkSheet*)sheet_);
  int row_number;
  if (if_from_file.empty()) {
    row_number = number_of_rows_;
  } else {
    row_number = if_from_file.size();
  }

  int rows_difference = row_number - last_rows_size;
  for (size_t i = 0; i < row_number; ++i) {
    std::ostringstream ostr;
    ostr << i + 1 << " from " << row_number << " rows gererated" << std::endl;
    gtk_label_set_text(((GtkLabel*)(progress_label_)), ostr.str().c_str());
    gtk_widget_show(progress_label_);
    if (counting_ == false) {
      break;
    }

    if (rows_difference > 0 && i > last_rows_size - 1) {
      gtk_sheet_add_row((GtkSheet*)sheet_, 1);
    }

    std::vector<std::string> row;
    if (if_from_file.empty()) {
      row = GenerateRandomRow();
    }
    else {
      std::istringstream str(if_from_file[i]);
      row.resize(columns_number);
      for (size_t i = 0 ; i < columns_number; ++i)
      {
      	str >> row[i];
      }

    }
    for (size_t j = 0; j < columns_number; ++j) {
      gtk_sheet_set_cell_text((GtkSheet*)sheet_, i, j, row[j].c_str());
    }
  }

  if (rows_difference < 0) {
    rows_difference = -rows_difference;
    gtk_sheet_delete_rows((GtkSheet*)sheet_, last_rows_size - rows_difference,
                          rows_difference);
  };

  counting_ = false;
  return;
};

Application::Application() : counting_(false), this_(this) {
  generate_button_ = gtk_button_new_with_label("Generate");
  window_main_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  horisontal_packing_window_ = gtk_hbox_new(FALSE, TRUE);
  vertical_packing_window_ = gtk_vbox_new(FALSE, TRUE);

  g_signal_connect(G_OBJECT(window_main_), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);
  g_signal_connect(G_OBJECT(window_main_), "delete_event",
                   G_CALLBACK(gtk_main_quit), NULL);

  scrolled_window_ = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(window_main_), horisontal_packing_window_);

  g_signal_connect(G_OBJECT(generate_button_), "clicked",
                   G_CALLBACK(GenerateButtonCallback), (gpointer)this);

  gtk_box_pack_start(GTK_BOX(horisontal_packing_window_), scrolled_window_, 1,
                     1, 1);

  entry_box_ = gtk_entry_new();
  error_label_ = gtk_label_new("Incorrect input");
  row_label_ = gtk_label_new("");
  progress_label_ = gtk_label_new("");

  file_select_ = gtk_button_new_with_label("Open");
  file_save_ = gtk_button_new_with_label("Save");

  g_signal_connect(G_OBJECT(file_select_), "clicked", G_CALLBACK(OpenButton),
                   (gpointer)this);

g_signal_connect(G_OBJECT(file_save_), "clicked", G_CALLBACK(SaveButton),
                   (gpointer)this);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), entry_box_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), generate_button_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), error_label_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), row_label_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), progress_label_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), file_select_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(vertical_packing_window_), file_save_, FALSE,
                     FALSE, FALSE);

  gtk_box_pack_start(GTK_BOX(horisontal_packing_window_),
                     vertical_packing_window_, FALSE, FALSE, FALSE);

  sheet_ = gtk_sheet_new(1, columns_number, "Edit table");

  gtk_container_add(GTK_CONTAINER(scrolled_window_), sheet_);

  gtk_signal_connect(GTK_OBJECT(sheet_), "select-row",
                     (GtkSignalFunc)(SheetActivatedRowCallback),
                     (gpointer)this);

  for (size_t i = 0; i < columns_number; ++i) {
    gtk_sheet_column_button_add_label((GtkSheet*)sheet_, i, column_names[i]);
  }

  GdkColor color3;
  gdk_color_parse("white", &color3);
  gtk_sheet_set_background((GtkSheet*)sheet_, &color3);
  gtk_widget_show_all(window_main_);
  gtk_widget_hide(error_label_);
  gtk_widget_hide(row_label_);
  gtk_widget_show(window_main_);

  gtk_main();
};

const std::vector<std::string> Application::GenerateRandomRow() {
  std::vector<std::string> row(columns_number);
  int random_int = rand() % max_generated_int;
  double random_double =
      ((double)(rand())) * 1.0 / RAND_MAX * max_generated_int;
  char random_char = alphabet[rand() % 26];
  std::ostringstream str_in;
  str_in << random_int << " " << random_double << " " << random_char;
  std::istringstream str_out(str_in.str());
  str_out >> row[0] >> row[1] >> row[2];
  (rand() % 100 / (100 * 1.0)) > 0.5 ? row[3] = "True" : row[3] = "False";
  return row;
}
