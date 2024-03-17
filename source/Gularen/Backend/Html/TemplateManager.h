#pragma once

#include "Gularen/Frontend/Node.h"
#include "Gularen/Backend/Html/Composer.h"
#include "Gularen/Library/Disk/File.h"
#include "Gularen/Library/HashTable.h"

namespace Gularen {
namespace Html {

class TemplateManager {
public:
	TemplateManager() {
	}

	void setDocument(Document* document) {
		_document = document;

		for (unsigned int j = 0; j < _document->annotations.size(); j += 1) {
			const Annotation& annotation = _document->annotations.get(j);
			_documentAnnotations.set(annotation.key, annotation.value);
		}
	}

	void setTemplateFile(const StringSlice path) {
		_templateContent = Disk::File::readAll(path);
	}

	StringSlice render() {
		_templateIndex = 0;

		while (_isBound(0)) {
			if (_get(0) == '<' && _isBound(4) &&
				_get(1) == '!' &&
				_get(2) == '-' &&
				_get(3) == '-' &&
				_get(4) == '[') {

				_advance(5);

				bool annotation = false;

				if (_isBound(0) && _get(0) == '[') {
					annotation = true;
					_advance(1);
				}

				unsigned int keyIndex = _templateIndex;

				while (_isBound(0) && _get(0) != ']') {
					_advance(1);
				}

				StringSlice key = StringSlice(_templateContent.pointer() + keyIndex, _templateIndex - keyIndex);

				if (_isBound(0) && _get(0) == ']') {
					_advance(1);
				}

				if (_isBound(0) && _get(0) == ']') {
					_advance(1);
				}

				if (_isBound(2) && _get(0) == '-' && _get(1) == '-' && _get(2) == '>') {
					_advance(3);
					if (annotation) {
						StringSlice* value = _documentAnnotations.get(key);
						if (value != nullptr) {
							_escape(*value);
						}
					} else {
						if (key == "content") {
							Composer composer;
							StringSlice content = composer.compose(_document);
							_content.append(content.pointer(), content.size());
						} else if (key == "toc") {
							Composer composer;
							StringSlice content = composer.composeToc(_document);
							_content.append(content.pointer(), content.size());
						}
					}
				}

				continue;
			}

			_content.append(_get(0));
			_advance(1);
		}

		return StringSlice(_content.pointer(), _content.size());
	}

private:
	bool _isBound(unsigned int offset) {
		return _templateIndex + offset < _templateContent.size();
	}

	char _get(unsigned int offset) {
		return _templateContent.get(_templateIndex + offset);
	}

	void _advance(unsigned int offset) {
		_templateIndex += offset;
	}

	void _escape(StringSlice in) {
		for (unsigned int i = 0; i < in.size(); i += 1) {
			switch (in.get(i)) {
				case '<': _content.append("&lt;"); break;
				case '>': _content.append("&gt;"); break;
				case '&': _content.append("&amp;"); break;
				case '\"': _content.append("&quot;"); break;
				case '\'': _content.append("&#39;"); break;
				default: _content.append(in.get(i)); break;
			}
		}
	}

private:
	String _templateContent;

	unsigned int _templateIndex;

	HashTable<StringSlice> _documentAnnotations;

	Document* _document;

	String _content;
};

}
}
