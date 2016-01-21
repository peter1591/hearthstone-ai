SUBDIRS = lib

all: $(SUBDIRS)

clean: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "===>" $@
		$(MAKE) -C $@ $(MAKECMDGOALS);
	@echo "<===" $@
