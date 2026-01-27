

MODULES = preprocessor \
          lexer        \
					analyzer     \
					depender     \
					optimizer    \
					builder

SRC  = ../src
SRCS = $(patsubst %,$(SRC)%.c,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_src = $(SRC)/$(module).c))

INC  = ../inc
INCS = $(patsubst %,$(INC)/%.h,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_inc = $(INC)/$(module).h))

OBJ  = ../obj
OBJS = $(patsubst %,$(OBJ)/%.o,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_obj = $(OBJ)/$(module).o))

build: $(OBJS)

preprocessor_modules_dep =
lexer_modules_dep        = analyzer
analyzer_modules_dep     =
depender_modules_dep     =
optimizer_modules_dep    =
builder_modules_dep      =


define MODULE_FILES
$(module)_files = $($(module)_src) $($(module)_inc)
endef
$(foreach module,$(MODULES),$(eval $(MODULE_FILES)))


define MODULE_DEP_FILES
$(module)_dep_files = $($(module)_files) $($(module)_obj)
endef
$(foreach module,$(MODULES),$(eval $(MODULE_DEP_FILES)))

define MODULE_DEPS = 
$(module)_dep = $$(foreach _module,$($(module)_modules_dep),$$($$(_module)_dep_files) $($(module)_files))
endef
$(foreach module,$(MODULES),$(eval $(MODULE_DEPS)))

# Comp rules
define MODULE_COMP = 
$($(module)_obj): $($(module)_dep)
	@gcc -c -D__IMPLEMENT__ $($(module)_src) -I$(INC) -o $($(module)_obj)
endef
$(foreach module,$(MODULES),$(eval $(MODULE_COMP)))

print:
	@echo 'lexer_src:   $(lexer_src)  '
	@echo 'lexer_inc:   $(lexer_inc)  '
	@echo 'lexer_obj:   $(lexer_obj)  '
	@echo 'lexer_files: $(lexer_files)'
	@echo 'lexer_dep:   $(lexer_dep)  '
	@echo 'lexer_files: $(lexer_files)'

clean:
	@rm -f $(OBJS)


test1: ../test/test1.c $(OBJS)
	@gcc ../test/test1.c -I$(INC) $(OBJS) -o ../test/test1.out



