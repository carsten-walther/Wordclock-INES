/**
 * LocalStorageServiceController
 */
export class LocalStorageServiceController {

  /**
   * localStorage
   */
  private localStorage: any = window.localStorage;

  /**
   * get
   * @param key
   */
  get = (key: string): Promise<any> => {
    return new Promise((resolve, reject) => {
      try {
        if (this.localStorage) {
          const item = this.localStorage.getItem(key);
          resolve(JSON.parse(item));
        }
        resolve(undefined);
      } catch (err) {
        reject(`Couldnt get object: ${err}`);
      }
    });
  };

  /**
   * set
   * @param key
   * @param value
   */
  set = (key: string, value: any): Promise<void> => {
    return new Promise((resolve, reject) => {
      try {
        this.localStorage && this.localStorage.setItem(key, JSON.stringify(value));
        resolve();
      } catch (err) {
        reject(`Couldnt store object ${err}`);
      }
    });
  };

  /**
   * remove
   * @param key
   */
  remove = (key: string): Promise<void> => {
    return new Promise((resolve, reject) => {
      try {
        this.localStorage && this.localStorage.removeItem(key);
        resolve();
      } catch (err) {
        reject(`Couldnt remove object ${err}`);
      }
    });
  };
}

export const LocalStorageService = new LocalStorageServiceController();
