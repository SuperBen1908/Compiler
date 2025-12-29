

MODULES = preprocessor \
          lexer        \
					analyzer     \
					depender     \
					optimizer    \
					builder

preprocessor_dep = 
lexer_dep        = 
analyzer_dep     =
depender_dep     =
optimizer_dep    =
builder_dep      = 

SRCS = $(patsubst %,../src/%.c,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_src = ../src/$(module).c))

INCS = $(patsubst %,../inc/%.h,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_inc = ../inc/$(module).h))

OBJS = $(patsubst %,../obj/%.o,$(MODULES))
$(foreach module,$(MODULES),$(eval $(module)_obj = ../obj/$(module).o))


print:
	@echo $(lexer_src)
	@echo $(lexer_inc)
	@echo $(lexer_obj)

define COMP_MODULE = 
$(1): 
endef

$(foreach module,$(MODULES),$(eval $(call $(COMP_SRC),$(module))))



print2:
	@echo $(MODULES)
	@echo $(SRCS)
	@echo $(INCS)





